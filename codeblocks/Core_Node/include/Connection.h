#ifndef CONNECTION_INCLUDED
#define CONNECTION_INCLUDED

extern "C" {
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
}
#include <string>

class Connection
{
public:
    Connection(evutil_socket_t fd, struct bufferevent* bev, void* server);
    void send(const char* data, size_t numBytes);

    struct bufferevent* bev;
    evutil_socket_t fd;
    void* server;
};


inline Connection::Connection(evutil_socket_t fd, bufferevent* bev, void* server)
{
    this->bev = bev;
    this->fd = fd;
    this->server = server;
    //printf("Created connection with server ref: %p\n", server);
}

inline void Connection::send(const char* data, size_t numBytes)
{
    if(bufferevent_write(bev, data, numBytes) == -1) {//向bev 的输出buffer添加大小为numBytes的数据，数据首地址为data
        printf("Error while sending in Connection::send()\n");
    }
}


#endif // CONNECTION_INCLUDED
