#include <ulxmlrpcpp.h>
#include <ulxr_protocol.h>
#include <ulxr_tcpip_connection.h>
#include <ulxr_except.h>
#include <iostream>
#include <ulxr_sip_protocol.h>
#include <ulxr_response.h>
#include <ulxr_dispatcher.h>
#include <ulxr_signature.h>

using namespace ulxr;
using namespace std;


MethodResponse CuRegister(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st;

	st.addMember("cuId", Value("cuId",RpcString("12")));
	st.addMember("keepAlivePeriod", Value("keepAlivePeriod", Integer(10)));
	st.addMember("currentTime", Value("currentTime",RpcString("10:00:00")));

	resp.setResult(st);
	resp.setMethodName("CuRegister");

	return resp;
}

MethodResponse CuKeepAlive(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st;
	
	st.addMember("keepAlivePeriod", Value("keepAlivePeriod",RpcString("12")));

	resp.setResult(st);
	resp.setMethodName("CuKeepAlive");

	return resp;
}

MethodResponse GetDepResList(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("totalPkt", Value("totalPkt",Integer(12)));
	st.addMember("pktNum", Value("pktNum",Integer(9)));
	URL.addMember("id",Value("id",RpcString("123")));
	URL.addMember("name",Value("name",RpcString("name_user")));
	URL.addMember("parentId",Value("parentId",RpcString("parent_1")));
	group.addItem(Value("URL",URL));
	st.addMember("group",Value("group",group));

	resp.setResult(st);
	resp.setMethodName("GetDepResList");

	return resp;
}

MethodResponse GetVideoResList(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("totalPkt", Value("totalPkt",Integer(12)));
	st.addMember("pktNum", Value("pktNum",Integer(9)));
	URL.addMember("id",Value("id",RpcString("123")));
	URL.addMember("name",Value("name",RpcString("name_user")));
	URL.addMember("depId",Value("depId",RpcString("dep_1")));
	URL.addMember("pos",Value("pos",RpcString("pos_1")));
	URL.addMember("target",Value("target",RpcString("target_1")));
	URL.addMember("remark",Value("remark",RpcString("remark_1")));
	group.addItem(Value("URL",URL));
	st.addMember("group",Value("group",group));

	resp.setResult(st);
	resp.setMethodName("GetVideoResList");

	return resp;
}

MethodResponse GetAlarmResList(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("totalPkt", Value("totalPkt",Integer(12)));
	st.addMember("pktNum", Value("pktNum",Integer(9)));
	URL.addMember("id",Value("id",RpcString("123")));
	URL.addMember("name",Value("name",RpcString("name_user")));
	URL.addMember("description",Value("description",RpcString("desc")));
	
	
	
	group.addItem(Value("URL",URL));
	st.addMember("group",Value("group",group));

	resp.setResult(st);
	resp.setMethodName("GetAlarmResList");

	return resp;
}

MethodResponse QueryAlarmInfo(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct URL;

	URL.addMember("id",Value("id",RpcString("123")));
	URL.addMember("type",Value("type",RpcString("alarm")));
	URL.addMember("startTime",Value("startTime",RpcString("11:00:00")));
	URL.addMember("endTime",Value("endTime",RpcString("12:00:00")));
	URL.addMember("description",Value("description",RpcString("alarm information")));
	group.addItem(Value("URL",URL));
	
	URL.clear();

	URL.addMember("id",Value("id",RpcString("324")));
	URL.addMember("type",Value("type",RpcString("warning")));
	URL.addMember("startTime",Value("startTime",RpcString("13:00:00")));
	URL.addMember("endTime",Value("endTime",RpcString("14:00:00")));
	URL.addMember("description",Value("description",RpcString("warning information")));
	group.addItem(Value("URL",URL));

	resp.setResult(Value("group",group));

	resp.setMethodName("QueryAlarmInfo");

	return resp;
}

MethodResponse ReportAlarmRes(const ulxr::MethodCall &call) {
	MethodResponse resp;

	resp.setResult(Value());
	resp.setMethodName("ReportAlarmRes");

	return resp;
}

MethodResponse QueryAlarmRes(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	URL.addMember("id",Value("id",RpcString("123")));
	URL.addMember("type",Value("type",RpcString("type unknown")));
	URL.addMember("state",Value("state",RpcString("state unknown")));
	
	group.addItem(Value("URL",URL));
	st.addMember("group",Value("group",group));

	resp.setResult(st);
	resp.setMethodName("QueryAlarmRes");
	
	return resp;
}

MethodResponse AlarmHisQuery(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("totalNumber", Value("totalNumber",Integer(12)));
	st.addMember("currentNumber", Value("currentNumber",Integer(9)));
	URL.addMember("alarmId",Value("alarmId",RpcString("alarmId_1")));
	URL.addMember("type",Value("type",RpcString("type_1")));
	URL.addMember("description",Value("description",RpcString("desc")));
	URL.addMember("alarmStartTime",Value("alarmStartTime",RpcString("10:00:00")));
	URL.addMember("alarmEndTime",Value("alarmEndTime",RpcString("12:00:00")));
	URL.addMember("resId",Value("resId",RpcString("resId_1")));
	URL.addMember("startTime",Value("startTime",RpcString("11:00:00")));
	URL.addMember("endTime",Value("endTime",RpcString("11:32:00")));
	URL.addMember("size",Value("size",RpcString("125")));

	group.addItem(Value("URL",URL));
	st.addMember("group",Value("group",group));

	resp.setResult(st);
	resp.setMethodName("AlarmHisQuery");

	return resp;
}

MethodResponse PTZCtrl(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("resId", Value("resId",RpcString("res_1")));

	resp.setResult(st);
	resp.setMethodName("PTZCtrl");

	return resp;
}
MethodResponse PlayQuery(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("resId", Value("resId",RpcString("res_1")));
	st.addMember("totalNumber", Value("totalNumber",Integer(111))); 
	st.addMember("currentNumber", Value("currentNumber",Integer(1515)));

	URL.addMember("startTime",Value("startTime",RpcString("11:00:00")));
	URL.addMember("endTime",Value("endTime",RpcString("10:00:00")));
	URL.addMember("size",Value("size",RpcString("1333")));

	st.addMember("recoderURL",Value("recoderURL",URL));
	

	resp.setResult(st);
	resp.setMethodName("PlayQuery");

	return resp;
}

MethodResponse PlayOpen(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("cuId", Value("cuId",RpcString("cuId_1")));
	st.addMember("resId", Value("resId",RpcString("res_1")));
	st.addMember("sessionId", Value("sessionId",RpcString("session_1"))); 
	st.addMember("tcpIp", Value("tcpIp",RpcString("192.168.80.112"))); 
	st.addMember("tcpPort", Value("tcpPort",RpcString("5656")));

	resp.setResult(st);
	resp.setMethodName("PlayOpen");

	return resp;
}

MethodResponse PlayStart(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("sessionId", Value("sessionId",RpcString("session_1"))); 

	resp.setResult(st);
	resp.setMethodName("PlayStart");

	return resp;
}

MethodResponse HisPlayOpen(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("cuId", Value("cuId",RpcString("id_1")));
	st.addMember("resId", Value("resId",RpcString("res_1")));
	st.addMember("sessionId",Value("sessionId",RpcString("session_1")));
	st.addMember("tcpIp",Value("tcpIp",RpcString("192.168.80.112")));
	st.addMember("tcpPort",Value("tcpPort",RpcString("8080")));

	resp.setResult(st);
	resp.setMethodName("HisPlayOpen");

	return resp;
}

MethodResponse HisPlayStart(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("sessionId", Value("sessionId",RpcString("session_1"))); 

	resp.setResult(st);
	resp.setMethodName("HisPlayStart");

	return resp;
}

MethodResponse HisLoadOpen(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("cuId", Value("cuId",RpcString("id_1")));
	st.addMember("resId", Value("resId",RpcString("res_1")));
	st.addMember("sessionId",Value("sessionId",RpcString("session_1")));
	st.addMember("tcpIp",Value("tcpIp",RpcString("192.168.80.112")));
	st.addMember("tcpPort",Value("tcpPort",RpcString("8080")));

	resp.setResult(st);
	resp.setMethodName("HisLoadOpen");

	return resp;
}

MethodResponse HisLoadStart(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("sessionId", Value("sessionId",RpcString("session_1"))); 

	resp.setResult(st);
	resp.setMethodName("HisLoadStart");

	return resp;
}

MethodResponse PlayClose(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("sessionId", Value("sessionId",RpcString("session_1"))); 

	resp.setResult(st);
	resp.setMethodName("PlayClose");

	return resp;
}

MethodResponse PlayCtrl(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;
	
	st.addMember("sessionId", Value("sessionId",RpcString("session_1"))); 

	resp.setResult(st);
	resp.setMethodName("PlayCtrl");

	return resp;
}

MethodResponse ReportCamResState(const ulxr::MethodCall &call) {
	MethodResponse resp;
	Array group;
	Struct st, URL;

	resp.setResult(Value());
	resp.setMethodName("ReportCamResState");

	return resp;
}


int main(){

	try{
		TcpIpConnection conn (1,INADDR_ANY,9966);
		SipProtocol prot(&conn);
		Protocol *protocol = &prot;
		prot.setPersistent(true);
		Dispatcher server(&prot);

		server.addMethod(ulxr::make_method(CuRegister),
					 Signature() << ulxr::Struct(),
					 ULXR_PCHAR("CuRegister"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("CuRegister"));
		server.addMethod(ulxr::make_method(CuKeepAlive),
					 Signature() << ulxr::Struct(),
					 ULXR_PCHAR("CuKeepAlive"),
                     Signature() << ulxr::RpcString(),
                     ULXR_PCHAR("CuKeepAlive"));
		server.addMethod(ulxr::make_method(GetDepResList),
					 Signature() << ulxr::Struct(),
					 ULXR_PCHAR("GetDepResList"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("GetDepResList"));
		server.addMethod(ulxr::make_method(GetVideoResList),
					 Signature() << ulxr::Struct(),
					 ULXR_PCHAR("GetVideoResList"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("GetVideoResList"));
		server.addMethod(ulxr::make_method(GetAlarmResList),
					 Signature() << ulxr::Struct(),
					 ULXR_PCHAR("GetAlarmResList"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("GetAlarmResList"));
		server.addMethod(ulxr::make_method(QueryAlarmInfo),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("QueryAlarmInfo"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer()
					 <<ulxr::Array(),
                     ULXR_PCHAR("QueryAlarmInfo"));
		server.addMethod(ulxr::make_method(ReportAlarmRes),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("ReportAlarmRes"),
                     Signature() << ulxr::Array(),
                     ULXR_PCHAR("ReportAlarmRes"));
		server.addMethod(ulxr::make_method(QueryAlarmRes),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("QueryAlarmRes"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Array(),
                     ULXR_PCHAR("QueryAlarmRes"));
		server.addMethod(ulxr::make_method(AlarmHisQuery),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("AlarmHisQuery"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer()
					 <<ulxr::Struct(),
                     ULXR_PCHAR("AlarmHisQuery"));
		server.addMethod(ulxr::make_method(PTZCtrl),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("PTZCtrl"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer(),
                     ULXR_PCHAR("PTZCtrl"));
		server.addMethod(ulxr::make_method(PlayQuery),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("PlayQuery"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer()
					 <<ulxr::Struct(),
                     ULXR_PCHAR("PlayQuery"));
		server.addMethod(ulxr::make_method(PlayOpen),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("PlayOpen"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer()
					 <<ulxr::Integer()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("PlayOpen"));
		server.addMethod(ulxr::make_method(PlayStart),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("PlayStart"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("PlayStart"));
		server.addMethod(ulxr::make_method(HisPlayOpen),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("HisPlayOpen"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer(),
                     ULXR_PCHAR("HisPlayOpen"));
		server.addMethod(ulxr::make_method(HisPlayStart),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("HisPlayStart"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("HisPlayStart"));
		server.addMethod(ulxr::make_method(HisLoadOpen),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("HisLoadOpen"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer(),
                     ULXR_PCHAR("HisLoadOpen"));
		server.addMethod(ulxr::make_method(HisLoadStart),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("HisLoadStart"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("HisLoadStart"));
		server.addMethod(ulxr::make_method(PlayClose),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("PlayClose"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer()
					 <<ulxr::RpcString(),
                     ULXR_PCHAR("PlayClose"));
		server.addMethod(ulxr::make_method(PlayCtrl),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("PlayCtrl"),
                     Signature() << ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::RpcString()
					 <<ulxr::Integer(),
                     ULXR_PCHAR(" PlayCtrl"));
		server.addMethod(ulxr::make_method( ReportCamResState),
                     Signature() << ulxr::Struct(),
                     ULXR_PCHAR("ReportCamResState"),
                     Signature() << ulxr::Array(),
                     ULXR_PCHAR("ReportCamResState"));
		/*for(int i = 0; i < 19; i++){
			cout << "getsignature " <<server.getMethod(i)->getSignature(true,false) << endl;
		}*/
		while(1) {
			MethodCall call = server.waitForCall();
			cout <<call.getXml() << endl;
			MethodResponse resp = server.dispatchCall(call);
			prot.buildResponseHeader("192.168.80.111");
			
			server.sendResponse(resp);
			//prot.close();
		}

	} catch ( ConnectionException &e) {
		cout << e.what() << endl;
	}

#if 0
	MethodResponse resp;
	Struct st;
	Value id("id", Integer(0));
	Value name("name",RpcString("someone"));
	st.addMember("id",id);
	st.addMember("name",name);
	resp.setResult(st);
	resp.setCommand("register");
	cout << resp.getXml() << endl;
	
	//resp.setFault(0,"error");
	//cout << resp.getXml() << endl;
#endif
	return 0;
}

