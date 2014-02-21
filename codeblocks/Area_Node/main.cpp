#include "include/Area_node.h"
#include "include/Parameters.h"

using namespace std;

/***************   构造返回值的 回调函数实现   *****************/
//响应历史视频查询
respHistoryFile QueryHistoryFiles(struct reqHistoryFile req_hf)
{
    struct respHistoryFile result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success");
    strcpy(result_arg.args, "5000s");   //心跳周期

    //parameters .......
    result_arg.result = 10;
    result_arg.totalNumber = 9;
    result_arg.currentNumber = 5;
    strcpy(result_arg.group[0].startTime, "2014-01-24 12:30:35");
    strcpy(result_arg.group[0].endTime , "2014-01-24 14:30:24");
    strcpy(result_arg.group[0].size, "25");
    strcpy(result_arg.group[1].startTime, "12:30");
    strcpy(result_arg.group[1].endTime , "14:30");
    strcpy(result_arg.group[1].size, "25");
    result_arg.group_num = 2;

    return result_arg;
}

//响应请求实时媒体流
result_StartMediaReq StartMediaReq( struct StartMediaReq startMediaReq_arg)
{
    struct result_StartMediaReq result_arg;
    //仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.value, "success");

    //parameters .......
    strcpy(result_arg.sessionId, "200111");
    strcpy(result_arg.tcpIp, "192.168.1.110");
    strcpy(result_arg.tcpPort, "50000");

    return result_arg;
}
//响应停止实时媒体流
result StopMediaReq( struct StopMediaReq stopMediaReq_arg)
{
    struct result result_arg;
    //仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success");

    //parameters .......
    strcpy(result_arg.args, "20033");   //sessionid
    return result_arg;
}

//响应请求历史视频回放
result_StartPlayBack StartPlayBack( struct reqStartPlayBack req_args)
{
    struct result_StartPlayBack result_arg;
    //仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.result_value, "success");

    //parameters .......
    strcpy(result_arg.sessionId, "200111");
    strcpy(result_arg.tcpIp, "192.168.1.110");
    strcpy(result_arg.tcpPort, "50000");

    return result_arg;
}

//响应请求历史视频回放
result_ControlFileBack ControlFileBack( struct req_ControlFileBack req_args)
{
    struct result_ControlFileBack result_arg;
    //仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.result_value, "success");

    //parameters .......
    strcpy(result_arg.sessionId, "20022222");

    return result_arg;
}
//c.2.16 响应请求摄像机资源
result_ReqCamResState ReqCamResState(struct ReqCamResState req_args)
{

    struct result_ReqCamResState result_arg;
//仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.result_value, "success ReqCamResState");

    //parameters .......
    strcpy(result_arg.group[0].resId, "12331");
    strcpy(result_arg.group[0].state, "soso");
    strcpy(result_arg.group[1].resId, "12332");
    strcpy(result_arg.group[0].state, "ok");

    result_arg.urlNum = 2;

    return result_arg;
}

//c.2.18 用户动态信息获取
result_UserCurState GetUserCurState(struct UserCurState req_args)
{
    struct result_UserCurState result_arg;
//仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.result_value, "success GetUserCurState");
    //parameters .......
    strcpy(result_arg.muId, "181818");
    strcpy(result_arg.curUserId, "181818");
    strcpy(result_arg.userIp, "192.168,80,134");
    strcpy(result_arg.userState, "busy");

    strcpy(result_arg.group[0].id, "181818");
    strcpy(result_arg.group[0].name, "Jason");
    strcpy(result_arg.group[1].id, "1919191");
    strcpy(result_arg.group[0].name, "wade");

    result_arg.urlNum = 2;

    return result_arg;
}

//c.2.20 摄像机资源屏蔽
result SetUserCamManage(struct SetUserCamManage req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success SetUserCamManage");
    strcpy(result_arg.args, "cuId");

    return result_arg;
}

//c.2.23 响应 告警资源订阅
result AlarmResSubscribe(struct AlarmResSubscribe req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success AlarmResSubscribe");
    strcpy(result_arg.args, "muId");

    return result_arg;
}

//c.2.25 响应 订阅告警的信息查询
result_QueryAlarmRes QueryAlarmRes(struct QueryAlarmRes req_args)
{
    struct result_QueryAlarmRes result_arg;
//仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.result_value, "success QueryAlarmRes");

    //parameters .......

    strcpy(result_arg.group[0].id, "2525252525");
    strcpy(result_arg.group[0].type, "one");
    strcpy(result_arg.group[0].time, "2014-02-14 11:00:20");
    strcpy(result_arg.group[0].state, "1");
    result_arg.group[0].alarmHisRecord = 1;
    strcpy(result_arg.group[0].rtUrl.resId, "22222");
    strcpy(result_arg.group[0].rtUrl.time, "2016-02-14 11:00:20");
    strcpy(result_arg.group[1].id, "2525252525");
    strcpy(result_arg.group[1].type, "one");
    strcpy(result_arg.group[1].time, "2014-02-14 11:00:20");
    strcpy(result_arg.group[1].state, "1");
    result_arg.group[1].alarmHisRecord = 1;
    strcpy(result_arg.group[1].rtUrl.resId, "22222");
    strcpy(result_arg.group[1].rtUrl.time, "2016-02-14 11:00:20");
    result_arg.urlNum = 2;

    return result_arg;

}

//C.2.26 响应 告警信息查询
result_ReportAlarmInfo ReportAlarmInfo(struct ReportAlarmInfo req_args)
{
    struct result_ReportAlarmInfo result_arg;
//仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.result_value, "success ReportAlarmInfo");

        strcpy(result_arg.muId,  req_args.muId);

    //parameters .......
    strcpy(result_arg.group[0].id, "2525252525");
    strcpy(result_arg.group[0].type, "one");
    strcpy(result_arg.group[0].startTime, "2014-02-14 11:00:20");
    strcpy(result_arg.group[0].endTime, "2015-02-14 11:00:20");
        strcpy(result_arg.group[0].message, "hehhe");

    strcpy(result_arg.group[1].id, "2626262");
    strcpy(result_arg.group[1].type, "e");
    strcpy(result_arg.group[1].startTime, "2014-02-14 11:00:20");
    strcpy(result_arg.group[1].endTime, "2016-02-14 11:00:20");
        strcpy(result_arg.group[1].message, "hehehhe");

    result_arg.urlNum = 2;

    return result_arg;

}

//C.2.27 云镜控制
result ControlPTZ(struct ControlPTZ req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success ControlPTZ");
    strcpy(result_arg.args, "resId");

    return result_arg;
}

////C.2.28  响应 跨节点资源下发
result ResTransOrder(struct ResTransOrder req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success ResTransOrder");
    strcpy(result_arg.args, "muId");

    return result_arg;
}

////C.2.29  响应 跨节点资源变更
result ResChangeOrder(struct ResChangeOrder req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success ResChangeOrder");
    strcpy(result_arg.args, "muId");

    return result_arg;
}

//C.2.8 响应 实时流媒体传输 核心DDU->区域DDu
result INFO(struct INFO req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success INFO");
    strcpy(result_arg.args, "sessionId");

    return result_arg;
}

//C.2.11 响应 历史视频流媒体传输 核心DDU->区域DDu
result HisInfo(struct INFO req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success HisInfo");
    strcpy(result_arg.args, "sessionId");

    return result_arg;
}

//C.2.14 响应 历史视频下载流媒体传输 核心DDU->区域DDu
result HisLoadInfo(struct INFO req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success HisLoadInfo");
    strcpy(result_arg.args, "sessionId");

    return result_arg;
}

/***************   结束 返回值回调函数   *****************/

int main(int argc, char **argv)
{
    { //注册回调函数，响应核心 MU的请求，返回响应 xml_body 参数
        pcb_FunMap.QueryHistoryFiles = &QueryHistoryFiles;
        pcb_FunMap.StartMediaReq = &StartMediaReq;
        pcb_FunMap.StopMediaReq = &StopMediaReq;
        pcb_FunMap.StartPlayBack = &StartPlayBack;
        pcb_FunMap.ControlFileBack = &ControlFileBack;
        pcb_FunMap.ReqCamResState = &ReqCamResState;
        pcb_FunMap.GetUserCurState = & GetUserCurState;
        pcb_FunMap.SetUserCamManage = &SetUserCamManage;
        pcb_FunMap.AlarmResSubscribe = &AlarmResSubscribe;
        pcb_FunMap.QueryAlarmRes = &QueryAlarmRes;
        pcb_FunMap.ReportAlarmInfo = &ReportAlarmInfo;
        pcb_FunMap.ControlPTZ = &ControlPTZ;
        pcb_FunMap.ResTransOrder = &ResTransOrder;
        pcb_FunMap.ResChangeOrder = &ResChangeOrder;

        pcb_FunMap.INFO = &INFO;
        pcb_FunMap.HisInfo = &HisInfo;
        pcb_FunMap.HisLoadInfo = &HisLoadInfo;
    }

    Area_node *ac = new Area_node("127.0.0.1", 1234);
    //连接服务器
    ThreadedExecutor executor;
    executor.execute(ac);
    //填充 sip 头部, 供发送到 核心 MU时使用
    (*ac).create_request_header("sip:127.0.0.1", "alice", "bob", "tom", "0001");

/********************** 下面是测试  ******************************************/
    //调用对应的函数
    const char *command = "ReportAlarmRes";
    struct ReportAlarmRes req_args = {
            "2323232323", "muName-23",
        2,{
                {"id_1", "type_1 ", "2014-02-14 10:17:12",  "2014-02-14 12:17:12",  "", "3333333333", 1, "0", "nomarl", 1, {"resid",  "2014-02-14 11:17:12"} },
                {"id_2", "type_2 ", "2015-02-14 10:17:12",  "2015-02-14 12:17:12",  "", "3333333333", 1, "1", "erro", 1, {"resid_2",  "2015-02-14 11:17:12"} },
        }
    };
    int co;

    while(cin>>co)
    {
        (*ac).ReportAlarmRes(command, &req_args);
    }

    return 0;
}


