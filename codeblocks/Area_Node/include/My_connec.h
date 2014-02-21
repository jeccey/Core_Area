#ifndef My_connec_H
#define My_connec_H

extern "C" {
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
}
#include <string>
#include "XML_Parse.h"
#include "Client.h"
#include "Message_handler.h"


class My_connec
{
public:
    My_connec(evutil_socket_t fd, struct bufferevent* bev)
    {
        this->bev = bev;
        this->fd = fd;

        //std::cout<<bev<<endl<<fd<<endl;
//-------------------------------------------------------------------------------------------------
        msghdler = new Message_handler();
        XMLMessageParse* pObj= new XMLMessageParse();

         {
        //初始化， 注册不同 command 参数所对应的解析xml的回调函数
        dealMsg.addMessageFunction("QueryHistoryFiles", &XMLMessageParse::QueryHistoryFiles, pObj);
        //dealMsg.addMessageFunction ("ResReport", &XMLMessageParse::ResReport, pObj);
        //dealMsg.addMessageFunction ("ResChange", &XMLMessageParse::ResChange, pObj);
         }
    }

    virtual ~My_connec() {};
#if 0
    void send(const char* data, size_t numBytes)
    {
        printf("================================");

        if(bufferevent_write(bev, data, numBytes) == -1)  //向bev 的输出buffer添加大小为numBytes的数据，数据首地址为data
        {
            printf("Error while sending in Connection::send()\n");
        }

    }
#endif
    void onRead(const char* data, const size_t& numBytes)
    {
        std::cout<<"++++++++++++ happy server request msg:++++++++++++++++++"<<std::endl<<data;

        if(bufferevent_write(bev, data, numBytes) == -1)  //向bev 的输出buffer添加大小为numBytes的数据，数据首地址为data
        {
            printf("Error while sending in Connection::send()\n");
        }


    }

    struct bufferevent* bev;
    evutil_socket_t fd;
    void* client;
protected:
    Message_handler *msghdler;
    XMLMessageParse* pObj;
    DealMessage dealMsg;
private:
    struct sip_message sip_msg;
    long cseq;
};

#endif // My_connec_H



