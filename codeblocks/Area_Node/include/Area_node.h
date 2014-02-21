#ifndef AREA_NODE_H
#define AREA_NODE_H

#include "Message_handler.h"
#include "Parameters.h"
#include "Area_callback.h"

#include "XML_Parse.h"
#include "Client.h"
#include "TCP_Client.h"

#include <zthread/Runnable.h>
#include <zthread/Thread.h>
#include <zthread/ThreadedExecutor.h>

#include <string>
#include <iostream>

using namespace  ZThread;

class Area_node:public Runnable
{
public:
    Area_node(const char *cip, const unsigned short cport);
    virtual ~Area_node();

    void run();

    void create_request_header(char *uri, char *via, char *to, char* from, char *call_id);
    std::string onRead(const char* data, const size_t& numBytes);
    void send_to_server(const char* data, const size_t& numBytes);

    //C.2.5　SIP注册  区域节点向核心节点注册
    void MURegister(const char *command, const struct MUInfo *ca);
    //C.2.2　区域节点向核心节点目录资源上报
    void ResReportArgs(const char *command, const struct ResReportArgs *resRpArgs);
    //C.2.3　目录资源变更
    void ResChange(const char *command, const struct ResChangeArgs *resChangeArgs) ;
    //C.2.6　心跳
    void MUKeepAlive(const char *command, const struct MUKeepAlive *muKeepAlive);
    //C.2.15　区域节点向核心节点目录资源上报
    void ReportCamResState(const char *command, const struct ReportCamResState *args);
    //C.2.17 用户资源上报
    void UserResReport(const char *command, const struct UserResReport *args);
        //c.2.19 用户资源变更
    void UserResChange(const char *command, const struct UserResChange *args);
        //c.2.21 告警资源上报
    void AlarmResListReport(const char *command, const struct AlarmResList *args);
    //c.2.22 告警资源变更
    void AlarmResListChange(const char *command, const struct AlarmResListChange *args);

    //C.2.24 告警信息上报
    void ReportAlarmRes(const char *command, const struct ReportAlarmRes *args);


    /***************************************************************************/
    TCP_Client tcli;
    //Client<Area_node>  cli;
    Message_handler *msghdler;
    XMLMessageParse* pObj;
    DealMessage dealMsg;
protected:
private:
    const char *ip;
    unsigned short port;
    string sx_msg;
    struct sip_message sip_msg;
    long cseq;

};

#endif // AREA_NODE_H
