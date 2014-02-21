#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

/**
 *
 * Though be aware; if you need zero copy buffer handling you need
 * to change the way we handle the buffers.
 *
 */

extern "C" {
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
}

//#include "Connection.h"
#include <iostream>
#include <map>
using std::map;

template<class T>
class Server
{
public:

    Server();
    ~Server();

    bool setup(const char *ip, const unsigned short& port);
    void start();
    void update();
    void addConnection(evutil_socket_t fd, T* connection);
    void addFd_IP(struct sockaddr *saddr, evutil_socket_t fd);
    void sendToAllClients(const char* data, size_t len);

    void sendToClient(char *ip, const char* data, size_t len);
    void removeConnection(evutil_socket_t fd);

    static void listenerCallback(
        struct evconnlistener* listener
        ,evutil_socket_t socket
        ,struct sockaddr* saddr
        ,int socklen
        ,void* server
    );

    static void signalCallback(evutil_socket_t sig, short events, void* server);
    static void writeCallback(struct bufferevent*, void* server);
    static void readCallback(struct bufferevent*, void* connection);
    static void eventCallback(struct bufferevent*, short, void* server);

private:
    struct sockaddr_in sin;
    struct event_base* base; //setup函数创建 base
    struct event* signal_event;
    struct evconnlistener* listener;

    map<evutil_socket_t, T*> connections;
    map<std::string, evutil_socket_t> ip_fd;
};

template<class T>
Server<T>::Server()
    :base(NULL)
    ,listener(NULL)
    ,signal_event(NULL)
{
}


template<class T>
Server<T>::~Server()
{
    if(signal_event != NULL) {
        event_free(signal_event);
    }

    if(listener != NULL) {
        evconnlistener_free(listener);
    }

    if(base != NULL) {
        event_base_free(base);
    }
}

template<class T>
bool Server<T>::setup(const char *ip, const unsigned short& port)
{

    base = event_base_new();
    if(!base) {
        printf("Server: cannot create base.\n");
        return false;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip);
    listener = evconnlistener_new_bind(
                   base
                   ,Server::listenerCallback
                   ,(void*)this
                   ,LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE
                   ,-1
                   ,(struct sockaddr*)&sin
                   ,sizeof(sin)
               );

    if(!listener) {
        printf("Cannot create listener.\n");
        return false;
    }

    signal_event = evsignal_new(base, SIGINT, signalCallback, (void*)this);
    if(!signal_event || event_add(signal_event, NULL) < 0) {

        printf("Cannog create signal event.\n");
        return false;
    }
    return true;
}

template<class T>
void Server<T>::start()
{
    if(base != NULL) {
        event_base_dispatch(base);
    }
}

template<class T>
void Server<T>::update()   //sever 定期调用该方法更新
{
    if(base != NULL) {
        //event_base_dispatch的更灵活版本。默认情况下，
        //这个循环会一直运行，直到没有添加的事件，或者直到调用了event_base_loopbreak()或者evenet_base_loopexit()
        event_base_loop(base, EVLOOP_NONBLOCK);
    }
}

template<class T>
void Server<T>::addConnection(evutil_socket_t fd, T* connection)
{
    //用pair 讲两个数据（fd， 对象指针）合成一个数据，插入到map容器 connections中
    connections.insert(std::pair<evutil_socket_t, T*>(fd, connection));
}

template<class T>
void Server<T>::addFd_IP(struct sockaddr *saddr, evutil_socket_t fd)
{
    struct sockaddr_in *b;
    b=(struct sockaddr_in *)saddr;
    std::string ip(inet_ntoa(b->sin_addr));
    ip_fd.insert(std::pair<std::string, evutil_socket_t>(ip, fd));
}



template<class T>
void Server<T>::removeConnection(evutil_socket_t fd)
{
    connections.erase(fd);  //map<evutil_socket_t, T*> connections;
    //map<std::string, evutil_socket_t> ip_fd;
    //再删除 ip_fd 中的对应项
    typename map<std::string, evutil_socket_t>::iterator ip_it = ip_fd.begin();
    while(ip_it  != ip_fd.end()) {
        if(ip_it->second == fd) {
            ip_fd.erase(ip_it->first);
        }
        ip_it++;
    }
}

template<class T>
void Server<T>::sendToAllClients(const char* data, size_t len)
{
    typename map<evutil_socket_t, T*>::iterator it = connections.begin();
    while(it != connections.end()) {
        it->second->send(data, len);
        ++it;
    }
}

template<class T>
void Server<T>::sendToClient(char *ip, const char* data, size_t len)
{
    typename map<std::string, evutil_socket_t>::iterator ip_it = ip_fd.begin();
    typename map<evutil_socket_t, T*>::iterator it = connections.begin();
    ip_it = ip_fd.find(ip);
    if(ip_it != ip_fd.end()) {
        it = connections.find(ip_it->second);
        it->second->send(data, len);
    } else {
        printf("not foudn\n");
    }
}

// -----------------------   static 回调函数  -------------------------------------------------------

template<class T>
void Server<T>::listenerCallback(
    struct evconnlistener* listener
    ,evutil_socket_t fd
    ,struct sockaddr* saddr
    ,int socklen
    ,void* data
)
{
    Server<T>* server = static_cast<Server<T>* >(data);				//server对象指针， 指向server
    struct event_base* base = (struct event_base*) server->base;	//server对象的 event base 指针
    struct bufferevent* bev;

    //bufferevent_socket_new创建基于套接字的 bufferevent
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(!bev) {
        event_base_loopbreak(base);
        printf("Error constructing bufferevent!\n");
        return;
    }

    T* conn = new T(fd, bev, (void*)server);                        //创建T类型动态对象， conn 为对象指针， 和Server类为关联关系
    server->addConnection(fd, conn);
    server->addFd_IP(saddr, fd);


    bufferevent_setcb(bev, Server::readCallback, Server::writeCallback,
                      Server::eventCallback, (void*)conn);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_enable(bev, EV_READ);
}

template<class T>
void Server<T>::signalCallback(evutil_socket_t sig, short events, void* data)
{
    Server<T>* server = static_cast<Server<T> *>(data);
    struct event_base* base = server->base;
    struct timeval delay = {2,0};
    printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");
    event_base_loopexit(base, &delay);
}

template<class T>
void Server<T>::writeCallback(struct bufferevent* bev, void* data)  //输出缓冲区中的data被清空时调用
{
    //暂时用不到 writeCallback
#if 0
    struct evbuffer* output = bufferevent_get_output(bev);

    if(evbuffer_get_length(output) == 0) {
        printf("write callback: data send over.\n");
    }
#endif
}

template<class T>
void Server<T>::readCallback(struct bufferevent* bev, void* connection)   //输入缓冲区有数据可读调用
{

    T* conn = static_cast<T*>(connection);                          //conn 为 T 类型的对象指针
    struct evbuffer* inputbuf = bufferevent_get_input(bev);         //获取输入缓冲区
    char readbuf[10240];
    size_t readn = 0;

    evutil_socket_t fd = bufferevent_getfd(bev);    //获取 该bufferevent 对应的客户端 fd

    //evbuffer_remove() 函数将复制和删除前面的inputbuf的sizeof(readbuf)个字节到内存中的data中及readbuf中。
    //如果少于datlen个字节可用，函数将复制所有的字节。失败返回的值是-1，否则返回复制的字节数
    memset(readbuf,  '\0', sizeof(readbuf));
    while( (readn = evbuffer_remove(inputbuf, &readbuf, sizeof(readbuf))) > 0) {
        conn->onRead(readbuf, readn);                              //调用 coon 所指对象的 onRead 方法
        //printf("--------------recv data from fd=%u:-----------------\n%s\n", fd, readbuf);
    }
}

template<class T>
void Server<T>::eventCallback(struct bufferevent* bev, short events, void* data)
{
    T* conn = static_cast<T*>(data);
    Server<T>* server = static_cast<Server<T>* >(conn->server);

    if(events & BEV_EVENT_EOF) {
        server->removeConnection(conn->fd);
        bufferevent_free(bev);
        printf("ref: %p, Connection closed.\n", server);
    } else if(events & BEV_EVENT_ERROR) {
        printf("Got an error on the connection: %s\n", strerror(errno));
    } else {
        printf("unhandled.\n");
    }
}

#endif // SERVER_H_INCLUDED
