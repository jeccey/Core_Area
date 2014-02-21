#include "include/Core_node.h"

/************   构造返回值的 回调函数实现   *******************************/
//响应注册
result MURegister(struct MUInfo muinfo)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success");
    strcpy(result_arg.args, "5000s");   //心跳周期

    return result_arg;
}

//响应资源上报函数
result ResReport(struct ResReportArgs ResReportArgs)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success haha");
    return result_arg;
}

//响应资源变更
result ResChange(struct ResChangeArgs ResChangeArgs)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success ha");
    return result_arg;
}
//响应心跳
result MUKeepAlive(struct MUKeepAlive MUKeepAlive)
{

    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success mukeepalive");
    return result_arg;
}

//c.2.15 响应摄像机状态上报
result_ReportCamResState ReportCamResState(struct ReportCamResState req_args)
{

    struct result_ReportCamResState result_arg;
//仅作测试
    result_arg.result_code = 0;
    strcpy(result_arg.result_value, "success ReportCamResState");

    //parameters .......
    strcpy(result_arg.group[0].resId, "12331");
    strcpy(result_arg.group[1].resId, "12332");

    result_arg.urlNum = 2;

    return result_arg;
}

//c.2.17 响应资源 上报
result UserResReport(struct UserResReport req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success UserResReport");
    return result_arg;
}
//c.2.19 响应资源 变更
result UserResChange(struct UserResChange req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success UserResReport");
    strcpy(result_arg.args, "muId");

    return result_arg;
}

//c.2.21 响应告警资源上报
result AlarmResListReport(struct AlarmResList req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success AlarmResListReport");
    strcpy(result_arg.args,  req_args.muId);

    return result_arg;
}

//C.2.22 告警资源变更
result AlarmResListChange(struct AlarmResListChange req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success AlarmResListChange");
    strcpy(result_arg.args,  req_args.muId);
    return result_arg;
}
//C.2.24 告警信息上报
result ReportAlarmRes(struct ReportAlarmRes req_args)
{
    struct result result_arg;
//仅作测试
    result_arg.code = 0;
    strcpy(result_arg.value, "success ReportAlarmRes");
    strcpy(result_arg.args,  req_args.muId);
    return result_arg;
}

/************   结束--响应   *******************************/
int main()
{
    Core_node *cc = new Core_node("127.0.0.1", 1234);

    {
        /****   注册回调函数... ...响应区域 MU的请求，生成响应 xml的参数  ****/
        pcb_FunMap.MURgister = &MURegister;
        pcb_FunMap.ResReport = &ResReport;
        pcb_FunMap.ResChange = &ResChange;
        pcb_FunMap.MUKeepAlive = &MUKeepAlive;
        pcb_FunMap.ReportCamResState = &ReportCamResState;
        pcb_FunMap.UserResReport = &UserResReport;
        pcb_FunMap.UserResChange = &UserResChange;
        pcb_FunMap.AlarmResListReport = &AlarmResListReport;
        pcb_FunMap.AlarmResListChange = &AlarmResListChange;
        pcb_FunMap.ReportAlarmRes = &ReportAlarmRes;
    }

    //创建server
    ThreadedExecutor executor;
    executor.execute(cc);

    //核心 MU 主动向  区域 MU 发送时（比如 历史视频查询 QueryHistoryFiles），需要构造 sip头部
    (*cc).create_request_header("sip:127.0.0.1", "server_alice", "server_bob", "server_tom", "0001");

    /*******************************下面是测试 ******************************/

    const char *command = "INFO";
    const char *command1 = "HisInfo";
    const char *command2 = "HisLoadInfo";

    struct INFO req_args = {
        "28888888",  "22", "info-12", "2"
    };

    int tt;
    while(cin>>tt) {
        (*cc).INFO(command, &req_args, "127.0.0.1");

  //              (*cc).HisInfo(command1, &req_args, "127.0.0.1");
  //                      (*cc).HisLoadInfo(command2, &req_args, "127.0.0.1");

    }

    return 0;
}
