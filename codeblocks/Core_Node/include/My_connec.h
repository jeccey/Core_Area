#ifndef My_connec_H
#define My_connec_H

#include "Server.h"
#include "Connection.h"

#include "Message_handler.h"
#include "XML_Parse.h"


class My_connec : public Connection
{
public:
    My_connec(evutil_socket_t fd, struct bufferevent * bev, void* server);

    virtual ~My_connec();

    void onRead(const char* data, const size_t& numBytes);

protected:
    Message_handler *msghdler;
    XMLMessageParse* pObj;
    DealMessage dealMsg;
private:
    struct sip_message sip_msg;
    long cseq;
};

#endif // My_connec_H

