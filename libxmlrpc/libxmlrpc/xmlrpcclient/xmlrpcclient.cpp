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
	testcall.addParam(Value("group", group));

	return testcall;
}

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

	testcall.addParam(param1);
	testcall.addParam(param2);
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

MethodCall ReportCamResState() {
	MethodCall testcall("ReportCamResState");
	Struct URL;
	Array group;

	URL.addMember("resId",Value("resId",RpcString("resId_1")));
	URL.addMember("state",Value("state",RpcString("state_1")));
	group.addItem(Value("URL",URL));
	testcall.addParam(Value("group",group));

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
		Protocol *protocol = &prot;
		prot.setPersistent(true);
		ulxr::Requester client(&prot);

		/*prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(CuRegister(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(CuKeepAlive(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(GetDepResList(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(GetVideoResList(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(GetAlarmResList(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(QueryAlarmInfo(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(ReportAlarmRes(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(QueryAlarmRes(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(AlarmHisQuery(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(PTZCtrl(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(PlayQuery(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(PlayOpen(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(PlayStart(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(HisPlayOpen(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(HisPlayStart(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(HisLoadOpen(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(HisLoadStart(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(PlayClose(),"").getXml() << endl;

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		cout << client.call(PlayCtrl(),"").getXml() << endl;*/

		prot.buildRequestHeader(user,from,to,via,cseq,call_id);
		//client.call(ReportCamResState(),"");
		cout << client.call(ReportCamResState(),"").getXml() << endl;

		//ReportCamResState
	} catch ( ConnectionException &e) {
		cout << e.what() << endl;
	}
	return 0;
}

