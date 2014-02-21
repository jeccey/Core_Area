#ifndef CORE_NODE_H
#define CORE_NODE_H

#include "My_connec.h"
#include "Message_handler.h"

#include <zthread/Runnable.h>
#include <zthread/Thread.h>
#include <zthread/ThreadedExecutor.h>


using namespace ZThread;

class Core_node: public Runnable
{
public:
    Core_node(const char* ip, const unsigned short port);
    virtual ~Core_node();
    void run();

    void create_request_header(char *uri, char *via, char *to, char* from, char *call_id);

    //C.2.4　历史视频查询
    void QueryHistoryFiles(const char *command, const struct reqHistoryFile *args,  char* ip);
    //C.2.7 请求实时媒体流
    void StartMediaReq(const char *command, const struct StartMediaReq *args, char* ip);
    //C.2.9 停止视频传输
    void StopMediaReq(const char *command, const struct StopMediaReq *args, char* ip);
    //C.2.10 请求历史视频回放
    void StartPlayBack(const char *command, const struct reqStartPlayBack *args, char* ip);
    //C.2.12 历史视频播放控制
    void ControlFileBack(const char *command, const struct req_ControlFileBack *args, char* ip);
    //C.2.13 请求历史视频下载
    void StartHisLoad(const char *command, const struct reqStartPlayBack *args, char* ip);
    //C.2.16 摄像机状态查询
    void ReqCamResState(const char *command, const struct ReqCamResState *args, char* ip);
    //c.2.18 用户动态信息获取
    void GetUserCurState(const char *command, const struct UserCurState *args, char* ip);
    //c.2.20 摄像机资源屏蔽
    void SetUserCamManage(const char *command, const struct SetUserCamManage *args, char* ip);
    //C.2.23 告警资源订阅
    void AlarmResSubscribe(const char *command, const struct AlarmResSubscribe *args, char* ip);
    //C.2.25 订阅告警的信息查询
    void QueryAlarmRes(const char *command, const struct QueryAlarmRes *args, char* ip);
    //C.2.26 告警信息查询
    void ReportAlarmInfo(const char *command, const struct ReportAlarmInfo *args, char* ip);
    //C.2.27 云镜控制
    void ControlPTZ(const char *command, const struct ControlPTZ *args, char* ip);
    //C.2.28 跨节点资源下发
    void ResTransOrder(const char *command, const struct ResTransOrder *args, char* ip);
    //C.2.29 跨节点资源变更
    void ResChangeOrder(const char *command, const struct ResChangeOrder *args, char* ip);

    //C.2.8 实时流媒体传输
    void INFO(const char *command, const struct INFO *args, char* ip);
    //C.2.11 历史视频媒体传输
    void HisInfo(const char *command, const struct INFO *args, char* ip);
    //c.2.14 历史视频下载传输
    void HisLoadInfo(const char *command, const struct INFO *args, char* ip);

protected:
private:
    const char *ip;
    unsigned short port;
    Server<My_connec>  server;
    struct sip_message sip_msg;
    Message_handler *msghdler;

    long cseq;
};

#endif // CORE_NODE_H

