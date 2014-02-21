#ifndef LIBEVENT_CLIENT_H
#define LIBEVENT_CLIENT_H

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

class Libevent_client
{
public:
    Libevent_client();
    virtual ~Libevent_client();

    void setup(const char *ip, const unsigned short& port);
    void connect();
    void send(const char* data,  size_t numBytes);

    static void error_quit(const char *str);
    static void event_cb(struct bufferevent *bev, short events, void *ptr);
    static void timeoutcb(evutil_socket_t fd, short what, void *arg);
    static void sock_readcb(struct bufferevent *bev, void *ptr);

    static int64_t total_bytes_read;
    static int64_t total_messages_read;


protected:
private:
    struct sockaddr_in servaddr;
    int res;
    int timeout_seconds;

    struct event_base *base;
    struct event *evtimeout;
    struct timeval timeout;
    struct bufferevent *sockbev;
};

#endif // LIBEVENT_CLIENT_H
