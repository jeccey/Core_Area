#ifndef AREA_CALLBACK_H
#define AREA_CALLBACK_H

#include "Parameters.h"

//定义函数指针类型
typedef struct respHistoryFile (*pcb_QueryHistoryFiles)( struct reqHistoryFile req_hf);
typedef struct result_StartMediaReq (*pcb_StartMediaReq)( struct StartMediaReq startMediaReq);
typedef struct result (*pcb_StopMediaReq)( struct StopMediaReq stopMediaReq);
typedef struct result_StartPlayBack (*pcb_StartPlayBack)(struct reqStartPlayBack req_args);
typedef struct result_ControlFileBack (*pcb_ControlFileBack)(struct req_ControlFileBack req_args);
typedef struct result_StartPlayBack (*pcb_StartHisLoad)(struct reqStartPlayBack req_args);
typedef struct result_ReqCamResState (*pcb_ReqCamResState )(struct ReqCamResState  req_args);
typedef struct result_UserCurState(*pcb_GetUserCurState)(struct UserCurState req_args);
typedef struct result(*pcb_SetUserCamManage)(struct SetUserCamManage req_args);
typedef struct result(*pcb_AlarmResSubscribe)(struct AlarmResSubscribe req_args);
typedef struct result_QueryAlarmRes(*pcb_QueryAlarmRes)(struct QueryAlarmRes req_args);
typedef struct result_ReportAlarmInfo(*pcb_ReportAlarmInfo)(struct ReportAlarmInfo req_args);
typedef struct result(*pcb_ControlPTZ)(struct ControlPTZ req_args);
typedef struct result(*pcb_ResTransOrder)(struct ResTransOrder req_args);
typedef struct result(*pcb_ResChangeOrder)(struct ResChangeOrder req_args);

typedef struct result(*pcb_INFO)(struct INFO req_args);
typedef struct result(*pcb_HisInfo)(struct INFO req_args);
typedef struct result(*pcb_HisLoadInfo)(struct INFO req_args);

struct pcbFunMap{
    pcb_QueryHistoryFiles QueryHistoryFiles;
    pcb_StartMediaReq StartMediaReq;
    pcb_StopMediaReq StopMediaReq;
    pcb_StartPlayBack StartPlayBack;
    pcb_ControlFileBack ControlFileBack;
    pcb_StartHisLoad StartHisLoad;
    pcb_ReqCamResState ReqCamResState;
    pcb_GetUserCurState GetUserCurState;
    pcb_SetUserCamManage SetUserCamManage;
    pcb_AlarmResSubscribe AlarmResSubscribe;
    pcb_QueryAlarmRes QueryAlarmRes;
    pcb_ReportAlarmInfo ReportAlarmInfo;
    pcb_ControlPTZ ControlPTZ;
    pcb_ResTransOrder ResTransOrder;
    pcb_ResChangeOrder ResChangeOrder;

    pcb_INFO INFO;
    pcb_HisInfo HisInfo;
    pcb_HisLoadInfo HisLoadInfo;
};
extern struct pcbFunMap pcb_FunMap;


#endif // AREA_CALLBACK_H
