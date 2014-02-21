/********   构造返回值的回调函数 生命*******************/
#ifndef CORE_CALLBACK_H
#define CORE_CALLBACK_H

#include "Parameters.h"

//定义函数原型指针类型
typedef struct result (*pcb_MURegister)( struct MUInfo muinfo);
typedef struct result (*pcb_ResReport)( struct ResReportArgs ResReportArgs);
typedef struct result (*pcb_ResChange)( struct ResChangeArgs ResChangeArgs);
typedef struct result (*pcb_MUKeepAlive)( struct MUKeepAlive MUKeepAlive);
typedef struct result_ReportCamResState (*pcb_ReportCamResState)(struct ReportCamResState req_args);
typedef struct result (*pcb_UserResReport)(struct UserResReport req_args);
typedef struct result (*pcb_UserResChange)(struct UserResChange req_args);
typedef struct result (*pcb_AlarmResListReport)(struct AlarmResList req_args);
typedef struct result (*pcb_AlarmResListChange)(struct AlarmResListChange req_args);

typedef struct result (*pcb_ReportAlarmRes)(struct ReportAlarmRes req_args);

struct pcbFunMap {

    pcb_MURegister MURgister;
    pcb_ResReport ResReport;
    pcb_ResChange ResChange;
    pcb_MUKeepAlive MUKeepAlive;
    pcb_ReportCamResState ReportCamResState;
    pcb_UserResReport UserResReport;
    pcb_UserResChange UserResChange;
    pcb_AlarmResListReport AlarmResListReport;
    pcb_AlarmResListChange AlarmResListChange;

    pcb_ReportAlarmRes ReportAlarmRes;

};
extern struct pcbFunMap pcb_FunMap;



#endif // CORE_CALLBACK_H
