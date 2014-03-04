#include <ulxmlrpcpp.h>
#include <ulxr_protocol.h>
#include <ulxr_tcpip_connection.h>
#include <ulxr_except.h>
#include <iostream>
#include <ulxr_sip_protocol.h>
#include <ulxr_call.h>
#include <ulxr_requester.h>
#include <ulxr_signature.h>

using namespace ulxr;
using namespace std;

#if 0
MethodCall CuRegister() {
	MethodCall testcall("CuRegister");

	Value param1("cuPassword",RpcString("stupid password"));
	Value param2("cuUserId", RpcString("12"));
	Value param3("cuIp",RpcString("127.0.0.1"));
	Value param4("cuPort",RpcString("8080"));
	Value param5("version",RpcString("1.0"));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);

	return testcall;
}

MethodCall CuKeepAlive() {
	MethodCall testcall("CuKeepAlive");

	Value param1("keepAlivePeriod",RpcString("10"));

	testcall.addParam(param1);

	return testcall;
}

MethodCall GetDepResList() {
	MethodCall testcall("GetDepResList");

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));

	testcall.addParam(param1);
	testcall.addParam(param2);

	return testcall;
}

MethodCall GetVideoResList() {
	MethodCall testcall("GetVideoResList");

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("departId", RpcString("depart_23"));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);

	return testcall;
}

MethodCall GetAlarmResList() {
	MethodCall testcall("GetAlarmResList");

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));

	testcall.addParam(param1);
	testcall.addParam(param2);

	return testcall;
}
MethodCall QueryAlarmInfo() {
	MethodCall testcall ("QueryAlarmInfo");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("001"));
	testcall.addParam(param1);

	Value param2("cuUserId",RpcString("12"));
	testcall.addParam(param2);

	Value param3("queryLocation",Integer(0));
	testcall.addParam(param3);

	Value id("id", RpcString("id123"));
	Value type("type", RpcString("Alarm"));
	Value startTime("startTime", RpcString("10:00:01"));
	Value endTime("endTime", RpcString("11:00:01"));
	URL.addMember("id",id);
	URL.addMember("type",type);
	URL.addMember("startTime",startTime);
	URL.addMember("endTime",endTime);
	group.addItem(Value("URL",URL));

    URL.clear();
    URL.addMember("id",Value("id",RpcString("id_6666666")));
    URL.addMember("type",Value("type",RpcString("id_6666666")));
    URL.addMember("startTime",Value("startTime",RpcString("id_6666666")));
    URL.addMember("endTime",Value("endTime",RpcString("id_6666666")));
	group.addItem(Value("URL",URL));

    testcall.addParam(Value("group", group));

	return testcall;
}


MethodCall AlarmHisQuery() {
	MethodCall testcall("AlarmHisQuery");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("alarmId", RpcString("alarmId_1"));
	Value param4("type", RpcString("type_1"));
	Value param5("queryLocation", Integer(341));

	URL.addMember("startTime",Value("startTime",RpcString("10:00:00")));
	URL.addMember("endTime",Value("endTime",RpcString("12:00:00")));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);

	testcall.addParam(Value("queryURL", URL));
	return testcall;
}

MethodCall PTZCtrl() {
	MethodCall testcall("PTZCtrl");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("cmd", RpcString("cmd_1"));
	Value param5("param", RpcString("param_1"));
	Value param6("speed", Integer(123));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);
	testcall.addParam(param6);

	return testcall;
}

MethodCall PlayQuery() {
	MethodCall testcall("PlayQuery");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("queryLocation",Integer(123));

	URL.addMember("startTime",Value("startTime",RpcString("10:00:00")));
	URL.addMember("endTime",Value("endTime",RpcString("12:00:00")));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(Value("queryURL",URL));

	return testcall;
}

MethodCall PlayOpen() {
	MethodCall testcall("PlayOpen");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("mediaType",Integer(123));
	Value param5("linkMode",Integer(231));
	Value param6("targetIpAddr",RpcString("192.168.80.111"));
	Value param7("targetPort", RpcString("8080"));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);
	testcall.addParam(param6);
	testcall.addParam(param7);

	return testcall;
}

MethodCall PlayStart() {
	MethodCall testcall("PlayStart");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("sessionId",RpcString("session_1"));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);

	return testcall;
}

MethodCall HisPlayOpen() {
	MethodCall testcall("HisPlayOpen");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("startTime",RpcString("startTime"));
	Value param5("endTime",RpcString("endTime"));
	Value param6("linkMode",Integer(1));
	Value param7("targetIpAddr",RpcString("192.168.80.112"));
	Value param8("targetPort",RpcString("5656"));
	Value param9("locationFlag",Integer(2));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);
	testcall.addParam(param6);
	testcall.addParam(param7);
	testcall.addParam(param8);
	testcall.addParam(param9);

	return testcall;
}

MethodCall HisPlayStart() {
	MethodCall testcall("HisPlayStart");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("sessionId",RpcString("session_1"));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);

	return testcall;
}

MethodCall HisLoadOpen() {
	MethodCall testcall("HisLoadOpen");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("startTime",RpcString("startTime"));
	Value param5("endTime",RpcString("endTime"));
	Value param6("linkMode",Integer(1));
	Value param7("targetIpAddr",RpcString("192.168.80.112"));
	Value param8("targetPort",RpcString("5656"));
	Value param9("locationFlag",Integer(2));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);
	testcall.addParam(param6);
	testcall.addParam(param7);
	testcall.addParam(param8);
	testcall.addParam(param9);

	return testcall;
}

MethodCall HisLoadStart() {
	MethodCall testcall("HisLoadStart");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("sessionId",RpcString("session_1"));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);

	return testcall;
}

MethodCall PlayClose() {
	MethodCall testcall("PlayClose");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("stopFlag", Integer(1));
	Value param5("sessionId",RpcString("session_1"));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);

	return testcall;
}

MethodCall PlayCtrl() {
	MethodCall testcall("PlayCtrl");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));
	Value param3("resId", RpcString("resId_1"));
	Value param4("sessionId",RpcString("session_1"));
	Value param5("cmd",RpcString("cmd_1"));
	Value param6("param", Integer(1));

	testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(param3);
	testcall.addParam(param4);
	testcall.addParam(param5);
	testcall.addParam(param6);

	return testcall;
}
#endif
#if 0
//A.2.8 告警信息上报----与B类共用
MethodCall ReportAlarmRes() {
	MethodCall testcall("ReportAlarmRes");
	Struct URL;
	Array group;

	URL.addMember("id",Value("id",RpcString("id_1")));
	URL.addMember("type",Value("type",RpcString("warning")));
	URL.addMember("startTime", Value("startTime", RpcString("11:00:00")));
	URL.addMember("endTime", Value("endTime", RpcString("12:00:00")));
	URL.addMember("targetInfo", Value("targetInfo", RpcString("target_1")));
	URL.addMember("level", Value("level",Integer(1)));
	URL.addMember("state", Value("state",Integer(2)));
	URL.addMember("description", Value("description", RpcString("description_1")));

	group.addItem(Value("URL",URL));

	testcall.addParam(Value("group", group));

	return testcall;
}
//A.2.21 摄像机状态上报——与B类共用
MethodCall ReportCamResState() {
	MethodCall testcall("ReportCamResState");
	Struct URL;
	Array group;

	URL.addMember("resId",Value("resId",RpcString("resId_555555")));
	URL.addMember("state",Value("state",RpcString("state_1")));
	group.addItem(Value("URL",URL));

    URL.clear();
    URL.addMember("resId",Value("resId",RpcString("resId_6666666")));
	URL.addMember("state",Value("state",RpcString("state_6666666666")));
	group.addItem(Value("URL",URL));

    testcall.addParam(Value("group",group));

	return testcall;
}
//A.2.9 订阅告警的状态查询—— 与B类共用
MethodCall QueryAlarmRes() {
	MethodCall testcall("QueryAlarmRes");
	Struct URL;
	Array group;

	Value param1("cuId",RpcString("id_123"));
	Value param2("cuUserId", RpcString("user_1"));

	URL.addMember("id",Value("id",RpcString("id_1")));
	URL.addMember("type",Value("type",RpcString("warning")));
	URL.addMember("state",Value("state",RpcString("state unknown")));
	group.addItem(Value("URL",URL));
    URL.clear();
	URL.addMember("id",Value("id",RpcString("id_222222222222222")));
	URL.addMember("type",Value("type",RpcString("warning2222222")));
	URL.addMember("state",Value("state",RpcString("state unknown222222222")));
	group.addItem(Value("URL",URL));
	
    testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(Value("group", group));

	return testcall;
}
#endif
/********************************************************************************
 *      AreaNode ---> CoreNode
 *******************************************************************************/
#if 1
//B.2.24 告警信息上报----与A类共用
MethodCall ReportAlarmRes() {
    MethodCall in_parm("ReportAlarmRes");
    Struct URL, url;
    Array group;

    Value param1("muId", RpcString("muid_123"));
    Value param2("muName", RpcString("muName_sp"));

    url.addMember("resId", Value("resId", RpcString("resid_1")));
    url.addMember("time", Value("time", RpcString("2014-02-25 11:00:00")));

    URL.addMember("id", Value("id", RpcString("id_1")));
    URL.addMember("type", Value("type", RpcString("2")));
    URL.addMember("startTime", Value("startTime", RpcString("2014-02-25 11:00:00")));
    URL.addMember("endTime", Value("endTime", RpcString("2014-02-25 11:00:00")));
    URL.addMember("targetInfo", Value("targetInfo", RpcString("info_123")));
    URL.addMember("level", Value("level", Integer("1")));
    URL.addMember("state", Value("state", RpcString("2")));
    URL.addMember("description", Value("description", RpcString("info_123")));
    URL.addMember("alarmHisRecord", Value("alarmHisRecord", RpcString("warn")));

    URL.addMember("url", Value("url", url));
    group.addItem(Value("URL", URL));

    url.clear();
    url.addMember("resId", Value("resId", RpcString("resid_35")));
    url.addMember("time", Value("time", RpcString("2015-02-25 11:00:00")));
    URL.clear();
    URL.addMember("id", Value("id", RpcString("id_2")));
    URL.addMember("type", Value("type", RpcString("3")));
    URL.addMember("startTime", Value("startTime", RpcString("2015-02-25 11:00:00")));
    URL.addMember("endTime", Value("endTime", RpcString("2015-02-25 11:00:00")));
    URL.addMember("targetInfo", Value("targetInfo", RpcString("info_3123")));
    URL.addMember("level", Value("level", Integer("51")));
    URL.addMember("state", Value("state", RpcString("52")));
    URL.addMember("description", Value("description", RpcString("info_5123")));
    URL.addMember("alarmHisRecord", Value("alarmHisRecord", RpcString("warn5")));
    URL.addMember("url", Value("url", url));
    group.addItem(Value("URL", URL));


    in_parm.addParam(param1);
    in_parm.addParam(param2);
    in_parm.addParam(Value("group", group));

    return in_parm;
}
//B.2.15 摄像机状态上报——与A类共用
MethodCall ReportCamResState() {
	MethodCall testcall("ReportCamResState");
	Struct URL;
	Array group;

    Value param1("muId", RpcString("muId_12"));

	URL.addMember("resId",Value("resId",RpcString("resId_1233333333333333")));
	URL.addMember("state",Value("state",RpcString("state_1")));
	group.addItem(Value("URL",URL));
	URL.clear();
    URL.addMember("resId",Value("resId",RpcString("resId_44444444444444444444444")));
	URL.addMember("state",Value("state",RpcString("state_1")));
	group.addItem(Value("URL",URL));

    testcall.addParam(param1);
    testcall.addParam(Value("group",group));

	return testcall;
}
//B.2.25 订阅告警的信息查询—— 与A类共用
MethodCall QueryAlarmRes() {
	MethodCall testcall("QueryAlarmRes");
	Struct URL;
	Array group;

	Value param1("muId",RpcString("id_666666666666"));
	Value param2("muName", RpcString("6666666666666"));

	URL.addMember("id",Value("id",RpcString("id_1")));
	URL.addMember("type",Value("type",RpcString("warning")));
	group.addItem(Value("URL",URL));
    URL.clear();
	URL.addMember("id",Value("id",RpcString("id_222222222222222")));
	URL.addMember("type",Value("type",RpcString("warning2222222")));
	group.addItem(Value("URL",URL));

    testcall.addParam(param1);
	testcall.addParam(param2);
	testcall.addParam(Value("group", group));

	return testcall;
}
#endif
MethodCall MUKeepAlive() {
	MethodCall testcall("MUKeepAlive");

	Value param1("muKeepAlivePeriod",RpcString("200"));

	testcall.addParam(param1);
    return testcall;
}


int main(){
	string user = "110@192.168.80.125";
	string from = "192.168.80.111";
	string to = "192.168.80.125";
	string via = "192.168.80.125";
	string cseq = "1";
	string call_id = "qega444afa4g6aga4g";

	try{
		TcpIpConnection conn (0,"localhost",9966);
		SipProtocol prot(&conn);
	    //Protocol *protocol = &prot;
		prot.setPersistent(true);
		ulxr::Requester client(&prot);
//For test
		prot.buildRequestHeader(user,from,to,via,cseq,call_id);//设置 sip 头部
        cout<<"************ client rcv resopnse msg: ****************************"<<endl;

	    cout << client.call(QueryAlarmRes(),"").getXml() << endl;

	} catch ( ConnectionException &e) {
		cout << e.what() << endl;
	}
	return 0;
}

