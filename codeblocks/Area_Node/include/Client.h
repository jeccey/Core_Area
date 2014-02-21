
#ifndef CLIENT_H
#define CLIENT_H

extern "C" {
#include <event.h>
#include <event2/event.h>
#include <event2/bufferevent.h>

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
}

#include "Message_handler.h"
#include <string>
#include <iostream>


template<class T>
class Client
{
    public:
    Client();
    virtual ~Client();

    void setup(const char *ip, const unsigned short& port);
    void connect();
    void send(const char* data,  size_t numBytes);

    void onread(struct bufferevent *bev, void *ptr);

    void test()
    {
        printf("~~~~~~~~This is client test () fun~~~~~~~~~~\n");
    }

    static void error_quit(const char *str);
    static void event_cb(struct bufferevent *bev, short events, void *ptr);
//    static void timeoutcb(evutil_socket_t fd, short what, void *arg);
    static void sock_readcb(struct bufferevent *bev, void *ptr);

    static int64_t total_bytes_read;
    static int64_t total_messages_read;

protected:
private:
    struct sockaddr_in servaddr;
    int res;
 //   int timeout_seconds;

    struct event_base *base;
    struct event *evtimeout;
    struct timeval timeout;
    struct bufferevent *sockbev;
    Message_handler *msghdler;
};

template<class T>
Client<T>::Client():base(NULL), evtimeout(NULL)
{
     msghdler = new Message_handler();
}

template<class T>
Client<T>::~Client()
{
    if(evtimeout != NULL)
    {
        event_free(evtimeout);
    }
    if(base != NULL)
    {
        event_base_free(base);
    }
}

template<class T>
void Client<T>::setup(const char *ip, const unsigned short& port)
{
    /* 初始化连接地址 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    /*inet_pton 处理IPv4,将“点分十进制” －> “整数 */
    res = inet_pton(AF_INET, ip, &servaddr.sin_addr);
    if( res != 1 )
    {
        error_quit("inet_pton error");
    }

//    timeout_seconds = 60;
  //  timeout.tv_sec = timeout_seconds;
 //   timeout.tv_usec = 0;
    base = event_base_new();
    if (!base)
    {
        error_quit("Couldn't open event base");
    }
    //evtimeout = evtimer_new(base, timeoutcb, base);
    //evtimer_add(evtimeout, &timeout);
}

template<class T>
void Client<T>::connect()
{

    /* 连接服务器并监听 */
    //bufferevent_socket_new创建基于套接字的bufferevent， 参数为-1 表示并不为此 bufferevent 设置 socket
    if ((sockbev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE)) == NULL)
    {
        error_quit("bufferevent_socket_new error");
    }
    // 建立连接,并将socket设置为非阻塞,连接成功返回 0 失败返回 -1
    res = bufferevent_socket_connect(sockbev, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if ( res < 0 )
    {
        bufferevent_free(sockbev);
        error_quit("connect error");
    }

    //T* conn = new T();
    //T* conn = new T(fd, bev);//这个地方应该还是需要的！！！！！！！
    //为bufferevent event设置回调函数
    bufferevent_setcb(sockbev, sock_readcb, NULL, event_cb, (void*)base);
    //启用读写操作
    bufferevent_enable(sockbev, EV_READ);
    bufferevent_enable(sockbev, EV_WRITE);

    //开始事件循环
    event_base_dispatch(base);
}

template<class T>
void Client<T>::send(const char* data, size_t numBytes)
{
    if(bufferevent_write(sockbev, data, numBytes) == -1)
    {
        printf("Error while sending in Connection::send()\n");
    }
}

/* 发生了致命错误，输入错误信息，退出程序 */
template<class T>
void Client<T>::error_quit(const char *str)
{
    /*  perror()将上一个函数发生错误的原因输出到标准错误(stderr);
        参数 s 所指的字符串会先打印出,后面再加上错误原因字符串;
        此错误原因依照全局变量 errno 的值来决定要输出的字符串。
    */
    perror(str);
    exit(1);
}

#if 0
template<class T>
void Client<T>::timeoutcb(evutil_socket_t fd, short what, void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    printf("timeout\n");

    event_base_loopexit(base, NULL);
}
#endif

/* 事件回调函数，连接状态改变时回调的函数 */
template<class T>
void Client<T>::event_cb(struct bufferevent *bev, short events, void *ptr)
{
    struct event_base *tbase = (struct event_base*)ptr;
    if ( events & BEV_EVENT_CONNECTED)
    {
        /* We're connected to server. Ordinarily we'd do
               something here, like start reading or writing. */
        evutil_socket_t fd = bufferevent_getfd(bev);
        int one = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
        printf("Server is connected!\n");
    }
    else    //如果连接不成功的消息，就停止事件循环
    {
        bufferevent_free(bev);
        event_base_loopbreak(tbase);
        printf("Server connect erro!! or The connect have been shutdown: %X\n", events);
    }
}

/* 服务器传信息过来了
   this callback is invoked when there is data to read on bev.
*/
template<class T>
void Client<T>::sock_readcb(struct bufferevent *bev, void *ptr)
{
    T* conn = static_cast<T*>(ptr);                          //conn 为 T 类型的对象指针

    struct evbuffer* inputbuf = bufferevent_get_input(bev);         //获取输入缓冲区
    char readbuf[10240];
    size_t readn = 0;
    std::string send_sx_msg("test") ;

    //evbuffer_remove() 函数将复制和删除前面的inputbuf的sizeof(readbuf)个字节到内存中的data中及readbuf中。
    //如果少于datlen个字节可用，函数将复制所有的字节。失败返回的值是-1，否则返回复制的字节数
    memset(readbuf,  '\0', sizeof(readbuf));
    while( (readn = evbuffer_remove(inputbuf, &readbuf, sizeof(readbuf))) > 0)
    {
        send_sx_msg += readbuf;
        //conn->onRead(readbuf, readn);
        std::cout<<"client: send_sx_msg---------------"<<std::endl<<send_sx_msg<<endl;
    }
    // std::cout<<"client: send_sx_msg.length()---------------"<<std::endl<<send_sx_msg.length()<<endl;
        #if 1
        if(send_sx_msg.length()  >0 ){
                    if( bufferevent_write(bev, send_sx_msg.c_str(), send_sx_msg.length()) == -1){
                        printf("Error while sending in Connection::send()\n");
                    }
        }
        #endif
}
#endif // CLIENT_H