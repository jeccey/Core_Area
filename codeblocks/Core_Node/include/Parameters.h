#ifndef PARAMETERS_INCLUDED
#define PARAMETERS_INCLUDED
// 响应
struct result {
    int code;
    char value[34];
    char args[34];
};

//2.5 SIP注册——解析
struct MUInfo {
    char muId[34];
    char muIp[17];
    char muPort[34];
    char muName[34];
    char muPassword[34];
    char manufacturerld[34];
    char manufacturerName[34];
    char productVersion[34];
    char softwareVersion[34];
};

//2.2 目录资源上报
struct URL {
    char resId[34];
    char name[34];
    char location[66];
    char purpose[66];
    char information[258];

};
struct ResReportArgs {
    char muId[34];
    int totalPacketNum;
    int curPacketNum;
    struct URL group[101];
};
//2.3 目录资源变更
struct ResChangeArgs {
    char muId[34];
    char cmd[18];
    struct URL resources;
};


// 2.4 历史视频查询
struct reqHistoryFile {
    char resId[34];
    char userId[34];
    char userLevel[34];
    char cuId[34];
    char fromDate[34];
    char toDate[34];
};

struct hURL {
    char startTime[34];
    char endTime[34];
    char size[34];
};
struct respHistoryFile {
    int code;
    char value[34];
    char args[34];
    //-----------------------------
    int result;
    int totalNumber;
    int currentNumber;
    int group_num;
    struct hURL group[101];
};

//2.6 心跳
struct MUKeepAlive {
    char muKeepAlivePeriod[34];
};

//2.7 请求实时媒体流
struct StartMediaReq {
    char resId[34];
    char userId[34];
    char userLevel[34];
    int mediaType;
    int linkMode;
    char targetIpAddr[34];
    char targetPort[34];
    int flag;
};
struct result_StartMediaReq { //响应参数
    int result_code;
    char value[34];
    // 下面的不同
    char sessionId[34];
    char tcpIp[34];
    char tcpPort[34];
};

//2.9 停止视频传输
struct StopMediaReq {
    char sessionId[34];
    char resId[34];
    int stopFlag;
};

//2.10  请求历史视频回放
struct reqStartPlayBack {
    char resId[34];
    char userId[34];
    char userLevel[34];
    char startTime[34];
    char endTime[34];
    int linkMode;
    char targetIpAddr[34];
    char targetPort[34];
    int flag;
    int locationFlag;
};
struct result_StartPlayBack {
    int result_code;
    char result_value[34];

    char sessionId[34];
    char tcpIp[34];
    char tcpPort[34];
};

//2.12   历史视频播放控制
struct req_ControlFileBack {
    char sessionId[34];
    char resId[34];
    char cmd[34];
    int param;
};
struct result_ControlFileBack {
    int result_code;
    char result_value[34];

    char sessionId[34];
};
//2.13 请求历史视频下载——和2.10  请求历史视频回放参数一样

//2.15 摄像机状态上报
struct CamURL {
    char resId[34];
    char state[34];
};
struct ReportCamResState {
    char muId[34];
    int urlNum;
    struct CamURL group[101];
};

struct result_ReportCamResState {
    int result_code;
    char result_value[34];
    //
    int urlNum;
    struct CamURL group[101];
};
//2.16 摄像机状态查询
struct ReqCamResState {
    char muId[34];
    int urlNum;
    struct CamURL group[101];
};

struct result_ReqCamResState {
    int result_code;
    char result_value[34];
    //
    int urlNum;
    struct CamURL group[101];
};

//2.17 用户资源上报-------------------------------
struct user_URL {
    char id[34];
    char name[34];
};
struct UserResReport {
    char muId[34];
    char muName[34];
    int totalPkt;
    int pktNum;

    int urlNum;
    struct user_URL group[101];
};

//c.2.18 用户动态信息获取 GetUserCurState
struct UserCurState {
    char muId[34];
    char curUserId[34];
};
struct result_UserCurState {
    int result_code;
    char result_value[34];

    char muId[34];
    char curUserId[34];
    char userIp[34];
    char userState[34];
    int urlNum;
    struct user_URL group[101];
};

//C.2.19 用户资源变更---------------------------------
struct UserResChange {
    char muId[34];
    int totalPkt;
    int pktNum;
    int cmd;

    int urlNum;
    struct user_URL group[101];
};

//C.2.20 摄像机资源屏蔽--------------------------------
struct cam_url {
    char camId[34];
};
struct id_url {
    char id[34];
};

struct SetUserCamManage {
    char cuId[34];
    char cuLevel[34];
    int Action;
    char startTime[34];
    char endTime[34];
    char schduleCreatTime[34];

    int camurl_Num;
    struct cam_url group[101];
    int idurl_Num;
    struct id_url group_id[101];
};

//C.2.21 告警资源上报--------------------------------
struct Alarm_URL {
    char id[34];
    char name[34];
    char description[34];    //sometimes equals type
};
struct AlarmResList {
    char muId[34];
    char muName[34];
    int totalPkt;
    int pktNum;

    int urlNum;
    struct Alarm_URL group[101];
};

//C.2.22 告警资源变更--------------------------------
struct AlarmResListChange {
    char muId[34];
    char muName[34];
    char num[34];
    int totalPkt;
    int pktNum;
    int cmd;

    int urlNum;
    struct Alarm_URL group[101];
};

//C.2.23 告警资源订阅--------------------------------
struct AlarmResSubscribe {
    char muId[34];
    char muName[34];
    int action;

    int urlNum;
    struct Alarm_URL group[101];
};

//C.2.24 告警信息上报
struct rt_url {
    char resId[34];
    char time[34];

};
struct AlarmRes_URL {
    char id[34];
    char type[34];
    char startTime[34];
    char endTime[34];
    char time[34];
    char targetInfo[66];
    int level;
    char state[34];
    char description[34];
    int alarmHisRecord;

    struct rt_url rtUrl;
};
struct ReportAlarmRes {
    char muId[34];
    char muName[34];

    int urlNum;
    struct AlarmRes_URL group[101];
};

//C.2.25 订阅告警的信息查询
struct QueryAlarmRes {
    char muId[34];
    char muName[34];

    int urlNum;
    struct AlarmRes_URL group[101];
};
struct result_QueryAlarmRes {
    int result_code;
    char result_value[34];

    char muId[34];
    char muName[34];

    int urlNum;
    struct AlarmRes_URL group[101];
};
//C.2.26 告警信息查询
struct AlarmInfo_URL {
    char id[34];
    char type[34];
    char startTime[34];
    char endTime[34];
    char message[34];
};
struct ReportAlarmInfo {
    char muId[34];
    char muName[34];

    int urlNum;
    struct AlarmInfo_URL group[101];
};
struct result_ReportAlarmInfo {
    int result_code;
    char result_value[34];

    char muId[34];
    int urlNum;
    struct AlarmInfo_URL group[101];
};

//C.2.27 云镜控制
struct ControlPTZ {
    char resId[34];
    char userId[34];
    char userLevel[34];
    char cmd[34];
    char param[34];
    int speed;
};

//C.2.28 跨节点资源下发
struct ResTransOrder {
    char muId[34];
    char totalPacketNum[34];
    char curPacketNum[34];

    struct  URL resources;
};
//C.2.29 跨节点资源变更
struct ResChangeOrder {
    char muId[34];
    char cmd[18];

    struct  URL resources;
};
//-------------------------------------------------------------------------
//C.2.8 实时流媒体传输 核心DDU->区域DDu
struct INFO{
    char sessionId[34];
    char resId[34];
    char userId[34];
    char userLevel[34];
};
//C.2.11 历史视频媒体传输
//c.2.14 历史视频下载传输

#endif // PARAMETERS_INCLUDED

