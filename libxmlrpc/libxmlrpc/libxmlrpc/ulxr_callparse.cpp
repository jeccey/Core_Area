/**************************************************************************
               ulxr_callparse.cpp  -  parse xml-rpc method call
                             -------------------
    begin                : Wed Mar 13 2002
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: ulxr_callparse.cpp 1158 2009-08-30 14:34:24Z ewald-arnold $

 ***************************************************************************/

/**************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ***************************************************************************/

//#define ULXR_SHOW_TRACE
//#define ULXR_DEBUG_OUTPUT
//#define ULXR_SHOW_READ
//#define ULXR_SHOW_WRITE
//#define ULXR_SHOW_XML

#include <cstring>
#include <string>
#include <memory>
#include <string.h>

#define ULXR_NEED_EXPORTS
#include <ulxmlrpcpp.h>  // always first header

#include <ulxr_callparse.h>
#include <ulxr_except.h>
#include <iostream>



#define MAXSTRLEN 30

using namespace std;
using namespace xercesc;

namespace ulxr {

MethodCallParser::MethodCallParser() {
	parser[0] = &MethodCallParser::parseCuRegister;
	cmdMap.insert(pair<string,int>("CuRegister",0));

	parser[1] = &MethodCallParser::parseCuKeepAlive;
	cmdMap.insert(pair<string,int>("CuKeepAlive",1));

	parser[2] = &MethodCallParser::parseGetDepResList;
	cmdMap.insert(pair<string,int>("GetDepResList",2));

	parser[3] = &MethodCallParser::parseGetVideoResList;
	cmdMap.insert(pair<string,int>("GetVideoResList",3));

	parser[4] = &MethodCallParser::parseGetAlarmResList;
	cmdMap.insert(pair<string,int>("GetAlarmResList",4));

	parser[5] = &MethodCallParser::parseQueryAlarmInfo;
	cmdMap.insert(pair<string,int>("QueryAlarmInfo",5));

	parser[6] = &MethodCallParser::parseReportAlarmRes;
	cmdMap.insert(pair<string,int>("ReportAlarmRes",6));

	parser[7] = &MethodCallParser::parseQueryAlarmRes;
	cmdMap.insert(pair<string,int>("QueryAlarmRes",7));

	parser[8] = &MethodCallParser::parseAlarmHisQuery;
	cmdMap.insert(pair<string,int>("AlarmHisQuery",8));

	parser[9] = &MethodCallParser::parsePTZCtrl;
	cmdMap.insert(pair<string,int>("PTZCtrl",9));

	parser[10] = &MethodCallParser::parsePlayQuery;
	cmdMap.insert(pair<string,int>("PlayQuery",10));

	parser[11] = &MethodCallParser::parsePlayOpen;
	cmdMap.insert(pair<string,int>("PlayOpen",11));

	parser[12] = &MethodCallParser::parsePlayStart;
	cmdMap.insert(pair<string,int>("PlayStart",12));

	parser[13] = &MethodCallParser::parseHisPlayOpen;
	cmdMap.insert(pair<string,int>("HisPlayOpen",13));

	parser[14] = &MethodCallParser::parseHisPlayStart;
	cmdMap.insert(pair<string,int>("HisPlayStart",14));

	parser[15] = &MethodCallParser::parseHisLoadOpen;
	cmdMap.insert(pair<string,int>("HisLoadOpen",15));

	parser[16] = &MethodCallParser::parseHisLoadStart;
	cmdMap.insert(pair<string,int>("HisLoadStart",16));

	parser[17] = &MethodCallParser::parsePlayClose;
	cmdMap.insert(pair<string,int>("PlayClose",17));

	parser[18] = &MethodCallParser::parsePlayCtrl;
	cmdMap.insert(pair<string,int>("PlayCtrl",18));

	parser[19] = &MethodCallParser::parseReportCamResState;
	cmdMap.insert(pair<string,int>("ReportCamResState",19));

	parser[20] = &MethodCallParser::parseReportCamResState;
	cmdMap.insert(pair<string,int>("GetCamResState",20));

    /*******************************************************************
     *  AreaNode ---> CoreNode
     ******************************************************************/
	parser[21] = &MethodCallParser::parseMURegister;
	cmdMap.insert(pair<string,int>("MURegister",21));

	parser[22] = &MethodCallParser::parseResReport;
	cmdMap.insert(pair<string,int>("ResReport",22));

    parser[23] = &MethodCallParser::parseResChange;
    cmdMap.insert(pair<string, int>("ResChange", 23));

    parser[24] = &MethodCallParser::parseMUKeepAlive;
    cmdMap.insert(pair<string, int>("MUKeepAlive", 24));

    parser[25] = &MethodCallParser::parseReportCamResState;
    cmdMap.insert(pair<string, int>("ReportCamResState", 25));

    parser[26] = &MethodCallParser::parseUserResReport;
    cmdMap.insert(pair<string, int>("UserResReport", 26));

    parser[27] = &MethodCallParser::parseUserResChange;
    cmdMap.insert(pair<string, int>("UserResChange", 27));

    parser[28] = &MethodCallParser::parseAlarmResListReport;
    cmdMap.insert(pair<string, int>("AlarmResListReport", 28));

    parser[29] = &MethodCallParser::parseAlarmResListChange;
    cmdMap.insert(pair<string, int>("AlarmResListChange", 29));

    parser[30] = &MethodCallParser::parseReportAlarmRes;
    cmdMap.insert(pair<string, int>("ReportAlarmRes", 30));
    /*******************************************************************
     *  CoreNode ---> AreaNode
     ******************************************************************/
    parser[31] = &MethodCallParser::parseQueryHistoryFiles;
    cmdMap.insert(pair<string, int>("QueryHistoryFiles", 31));

    parser[32] = &MethodCallParser::parseStartMediaReq;
    cmdMap.insert(pair<string, int>("StartMediaReq", 32));

    parser[33] = &MethodCallParser::parseINFO;
    cmdMap.insert(pair<string, int>("INFO", 33));

    parser[34] = &MethodCallParser::parseStopMediaReq;
    cmdMap.insert(pair<string, int>("StopMediaReq", 34));

    parser[35] = &MethodCallParser::parseStartPlayBack;
    cmdMap.insert(pair<string, int>("StartPlayBack", 35));

    parser[36] = &MethodCallParser::parseHisInfo;
    cmdMap.insert(pair<string, int>("HisInfo", 36));

    parser[37] = &MethodCallParser::parseControlFileBack;
    cmdMap.insert(pair<string, int>("ControlFileBack", 37));

    parser[38] = &MethodCallParser::parseStartHisLoad;
    cmdMap.insert(pair<string, int>("StartHisLoad", 38));

    parser[39] = &MethodCallParser::parseHisLoadInfo;
    cmdMap.insert(pair<string, int>("HisLoadInfo", 39));

    parser[40] = &MethodCallParser::parseReqCamResState;
    cmdMap.insert(pair<string, int>("ReqCamResState", 40));

    parser[41] = &MethodCallParser::parseGetUserCurState;
    cmdMap.insert(pair<string, int>("GetUserCurState", 41));

    parser[42] = &MethodCallParser::parseSetUserCamManage;
    cmdMap.insert(pair<string, int>("SetUserCamManage", 42));

    parser[43] = &MethodCallParser::parseAlarmResSubscribe;
    cmdMap.insert(pair<string, int>("AlarmResSubscribe", 43));

    parser[44] = &MethodCallParser::parseQueryAlarmRes;
    cmdMap.insert(pair<string, int>("QueryAlarmRes", 44));

    parser[45] = &MethodCallParser::parseReportAlarmInfo;
    cmdMap.insert(pair<string, int>("ReportAlarmInfo", 45));

    parser[46] = &MethodCallParser::parseControlPTZ;
    cmdMap.insert(pair<string, int>("ControlPTZ", 46));

    parser[47] = &MethodCallParser::parseResTransOrder;
    cmdMap.insert(pair<string, int>("ResTransOrder", 47));

    parser[48] = &MethodCallParser::parseResChangeOrder;
    cmdMap.insert(pair<string, int>("ResChangeOrder", 48));
}

ULXR_API_IMPL(int) MethodCallParser::parse(CppString buffer)
{
	int ret = 0;
    DOMNode *rootNode;
	try {
		XMLPlatformUtils::Initialize();
	} catch (const XMLException &toCatch) {
		char *message = XMLString::transcode(toCatch.getMessage());
		cout << message << endl;
		XMLString::release(&message);
		return 0;
	}

	MemBufInputSource *pMemBufInputSource = new MemBufInputSource((const XMLByte *)buffer.c_str(), buffer.length(), (const XMLCh *)0);
	XercesDOMParser *pDomParser = new XercesDOMParser;
	pDomParser->parse(*pMemBufInputSource);
	xercesc::DOMDocument *doc = pDomParser->getDocument();
	DOMNodeList *nodeList = doc->getElementsByTagName(XMLString::transcode("request"));
	if(!nodeList || nodeList->getLength() > 1)
		goto end;
	rootNode = nodeList->item(0);
	if(rootNode->hasAttributes()) {
		DOMNamedNodeMap *nodeMap = rootNode->getAttributes();
		string nodeName;
		for(int i = 0; i < nodeMap->getLength(); i++) {
			nodeName = XMLString::transcode(nodeMap->item(i)->getNodeName());
			if (nodeName == "command") {
				call.setMethodName(XMLString::transcode(nodeMap->item(i)->getNodeValue()));
				DOMNode *childNode = rootNode->getFirstChild();
                //cout << call.getMethodName() << " "<< cmdMap[call.getMethodName()]<<endl;//for test
				ret = (this->*parser[cmdMap[call.getMethodName()]])(childNode);
				break;
			}
		}
	}

end:
	delete pMemBufInputSource;
	delete pDomParser;
	return ret;
}

Value MethodCallParser::getSimpleValue(DOMNode *parametersNode, string valueName, ValueType type) {
	Value val;
	CppString paramName;
	DOMNodeList *childNodes = parametersNode->getChildNodes();
	DOMNode *node, *textNode;
	string text;

	if(!childNodes) return Value();

	for(int i = 0; i < childNodes->getLength(); i++) {
		node = childNodes->item(i);
		paramName = XMLString::transcode(node->getNodeName());
		if(paramName != valueName) continue;

		textNode = node->getFirstChild();
		if(!textNode) return Value();
		text = XMLString::transcode(textNode->getNodeValue());
		switch(type) {
		case RpcInteger:
			val = Integer(text);
			break;
		case RpcStrType:
			val = RpcString(text);
			break;
		default:
			break;
		}
		break;
	}
	val.setValueName(valueName);

	return val;
}

Value MethodCallParser::getStructValue(DOMNode *parametersNode, string valueName, unordered_map<string,string> fieldValueMap, int index) {
	DOMNodeList *childNodes = parametersNode->getChildNodes();
	DOMNode *node;
	CppString paramName;
	Struct structValue;
	Value val, fieldValue;
	ValueType type;
	int count = 0;

	for(int i = 0; i < childNodes->getLength(); i++) {
		node = childNodes->item(i);
		paramName = XMLString::transcode(node->getNodeName());
        if(paramName != valueName) continue;
		if(count != index){
            count++;
            continue;
        }

		unordered_map<string,string>::iterator iter;
		for(iter = fieldValueMap.begin(); iter != fieldValueMap.end(); iter++) {
			if(iter->second == "int")
				type = RpcInteger;
			else if(iter->second == "string")
				type = RpcStrType;
			else
				return Value();
			fieldValue = getSimpleValue(node, iter->first, type);
			structValue.addMember(iter->first, fieldValue);
		}
		break;
	}
	val = structValue;
	val.setValueName(valueName);

	return val;
}

Value MethodCallParser::getArrayValue(DOMNode *parametersNode, string valueName, string itemName, unordered_map<string,string> itemFieldValueMap) {
	Array group;
	CppString paramName;
	DOMNodeList *childNodes = parametersNode->getChildNodes();
	DOMNode *node;
	int i;

	if(!childNodes) return Value();

	for(i = 0; i < childNodes->getLength(); i++) {
		node = childNodes->item(i);
		paramName = XMLString::transcode(node->getNodeName());
		if(paramName == valueName) break;
	}
	if(i >= childNodes->getLength()) return Value();

	DOMNodeList *items = node->getChildNodes();
	for(i = 0; i < items->getLength(); i++) {
		group.addItem(getStructValue(node, itemName, itemFieldValueMap, i));
	}
#if 0
    cout<<"----------------group.getxml()-------------------"<<endl;
	cout << group.getXml() << endl;
#endif
	return Value(valueName, group);
}
//活动B类告警信息上报的 Group;  valueName=group
Value MethodCallParser::getReportAlarmRes(DOMNode *parametersNode, string valueName, string itemName, unordered_map<string,string> urlFieldValueMap){

	DOMNodeList *childNodes = parametersNode->getChildNodes();
	if(!childNodes)	return Value();

	int i;
	DOMNode *node_group;
	CppString paramName;
	//node_group: find valueName, check if equal group
	for(i = 0; i < childNodes->getLength(); i++) {
		node_group = childNodes->item(i);
		paramName = XMLString::transcode(node_group->getNodeName());
		if(paramName == valueName)	break;
	}
	if(i >= childNodes->getLength())
		return Value();
	//*url_list: valueName包括的成员列表(e.g. URL...)
	DOMNodeList *url_list = node_group->getChildNodes();

	Struct URL, url;
	Array group;
	DOMNode *item;
	CppString item_name;
	string text;
	for(i = 0; i < url_list->getLength(); i++) {//解析每个 URL
		item = url_list->item(i);
        URL.clear();
		URL.addMember("id", getSimpleValue(item, "id", RpcStrType));
		URL.addMember("type", getSimpleValue(item, "type", RpcStrType));
		URL.addMember("startTime", getSimpleValue(item, "startTime", RpcStrType));
		URL.addMember("endTime", getSimpleValue(item, "endTime", RpcStrType));
		URL.addMember("targetInfo", getSimpleValue(item, "targetInfo", RpcStrType));
		URL.addMember("level", getSimpleValue(item, "level", RpcInteger));
		URL.addMember("state", getSimpleValue(item, "state", RpcStrType));
		URL.addMember("description", getSimpleValue(item, "description", RpcStrType));
		URL.addMember("alarmHisRecord", getSimpleValue(item, "alarmHisRecord", RpcStrType));

		url = getStructValue(item, "url", urlFieldValueMap);
		URL.addMember("url", Value("url", url));

        group.addItem(Value("URL", URL));
    }

	return Value("group", group);

}
///////////////////////////////////////////////////////////////////////////////////////////

int MethodCallParser::parseCuRegister(DOMNode * parametersNode) {
	Value val[5];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuPassword", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "cuIp", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "cuPort", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "version", RpcStrType);

	for(int i = 0; i < 5; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}

int MethodCallParser::parseCuKeepAlive(DOMNode *parametersNode) {
	Value val;
	if(!parametersNode) return 0;

	val = getSimpleValue(parametersNode, "keepAlivePeriod", RpcStrType);

	if(val.getType() == RpcVoid)
		return 0;
	call.addParam(val);

	return 1;
}

int MethodCallParser::parseGetDepResList(DOMNode * parametersNode) {
	Value val[2];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);

	for(int i = 0; i < 2; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}

int MethodCallParser::parseGetVideoResList(DOMNode *parametersNode) {
	Value val[3];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "departId", RpcStrType);

	for(int i = 0; i < 3; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
int MethodCallParser::parseGetAlarmResList(DOMNode *parametersNode) {
	Value val[2];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);

	for(int i = 0; i < 2; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}

int MethodCallParser::parseQueryAlarmInfo(DOMNode *parametersNode) {
	Value val[4];
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "queryLocation", RpcInteger);
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	fieldValueMap.insert(pair<string, string>("startTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	val[3] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//A类、B类接口共用__告警信息上报
int MethodCallParser::parseReportAlarmRes(DOMNode *parametersNode) {
	Value val;
    Value muId, muName;
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

    muId = getSimpleValue(parametersNode, "muId", RpcStrType);
    muName = getSimpleValue(parametersNode, "muName", RpcStrType);

	if(muId.getType() == RpcVoid && muName.getType() == RpcVoid){//A类
		fieldValueMap.insert(pair<string,string>("id", "string"));
		fieldValueMap.insert(pair<string,string>("type", "string"));
		fieldValueMap.insert(pair<string,string>("startTime", "string"));
		fieldValueMap.insert(pair<string,string>("targetInfo", "string"));
		fieldValueMap.insert(pair<string,string>("level", "int"));
		fieldValueMap.insert(pair<string,string>("state", "int"));
		fieldValueMap.insert(pair<string,string>("description", "string"));

		val = getArrayValue(parametersNode, "group", "URL", fieldValueMap);
		if(val.getType() == RpcVoid)
			return 0;
	    call.addParam(val);
    }else{//B类
        if(muId.getType() == RpcVoid || muName.getType() == RpcVoid)
            return 0;
        else{
            call.addParam(muId);
            call.addParam(muName);
			//fieldValueMap 为小url的
			fieldValueMap.insert(pair<string,string>("resId", "string"));
			fieldValueMap.insert(pair<string,string>("time", "string"));
			val = getReportAlarmRes(parametersNode, "group", "URL", fieldValueMap);//更换新的

			if(val.getType() == RpcVoid)
				return 0;

			call.addParam(val);
        }
    }

	return 1;
}

//A 类，B类接口共用
int MethodCallParser::parseQueryAlarmRes(DOMNode *parametersNode) {
	Value val[3];
    Value muId, muName;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
    muId = getSimpleValue(parametersNode, "muId", RpcStrType);
    muName = getSimpleValue(parametersNode, "muName", RpcStrType);

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id", "string"));
	fieldValueMap.insert(pair<string,string>("type", "string"));
	val[2] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

    if(muId.getType() == RpcVoid || muName.getType() == RpcVoid){//A类
	    for(int i = 0; i < 3; i++) {
		    if(val[i].getType() == RpcVoid)
		    	return 0;
		    call.addParam(val[i]);
	    }
    }else{ //B 类
        if(muId.getType() == RpcVoid || muName.getType() == RpcVoid || val[2].getType() == RpcVoid)
            return 0;
        else{
            call.addParam(muId);
            call.addParam(muName);
            call.addParam(val[2]);
        }
    }

	return 1;
}

int MethodCallParser::parseAlarmHisQuery(DOMNode *parametersNode) {
	Value val[6];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "alarmId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "type", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "queryLocation", RpcInteger);

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("starTime", "string"));
	fieldValueMap.insert(pair<string,string>("endTime", "string"));
	val[5] = getStructValue(parametersNode, "queryURL", fieldValueMap);

	for(int i = 0; i < 6; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parsePTZCtrl(DOMNode *parametersNode) {
	Value val[6];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "cmd", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "param", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "speed", RpcInteger);

	for(int i = 0; i < 6; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parsePlayQuery(DOMNode *parametersNode) {
	Value val[5];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "queryLocation", RpcInteger);
	
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("startTime", "string"));
	fieldValueMap.insert(pair<string,string>("endTime", "string"));
	val[4] = getStructValue(parametersNode, "queryURL", fieldValueMap);

	for(int i = 0; i < 5; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parsePlayOpen(DOMNode *parametersNode) {
	Value val[7];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "mediaType", RpcInteger);
	val[4] = getSimpleValue(parametersNode, "linkMode", RpcInteger);
	val[5] = getSimpleValue(parametersNode, "targetIpAddr", RpcStrType);
	val[6] = getSimpleValue(parametersNode, "targetPort", RpcStrType);
	
	for(int i = 0; i < 7; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parsePlayStart(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parseHisPlayOpen(DOMNode *parametersNode) {
	Value val[9];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "startTime", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "endTime", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "linkMode", RpcInteger);
	val[6] = getSimpleValue(parametersNode, "targetIpAddr", RpcStrType);
	val[7] = getSimpleValue(parametersNode, "targetPort", RpcStrType);
	val[8] = getSimpleValue(parametersNode, "locationFlag", RpcInteger);

	for(int i = 0; i < 9; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parseHisPlayStart(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parseHisLoadOpen(DOMNode *parametersNode) {
	Value val[9];
	if(!parametersNode) return 0;
	cout << "parseHisLoadOpen" << endl;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "startTime", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "endTime", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "linkMode", RpcInteger);
	val[6] = getSimpleValue(parametersNode, "targetIpAddr", RpcStrType);
	val[7] = getSimpleValue(parametersNode, "targetPort", RpcStrType);
	val[8] = getSimpleValue(parametersNode, "locationFlag", RpcInteger);

	for(int i = 0; i < 9; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parseHisLoadStart(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parsePlayClose(DOMNode *parametersNode) {
	Value val[5];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "stopFlag", RpcInteger);
	val[4] = getSimpleValue(parametersNode, "sessionId", RpcStrType);


	for(int i = 0; i < 5; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}

int MethodCallParser::parsePlayCtrl(DOMNode *parametersNode) {
	Value val[6];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "cmd", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "param", RpcInteger);
	for(int i = 0; i < 6; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}

// 摄像机状态上报—— A 类、B类接口共用
int MethodCallParser::parseReportCamResState(DOMNode *parametersNode) {
	Value val;
    Value val_muid;
	if(!parametersNode) return 0;

	unordered_map<string, string> fieldValueMap;

    val_muid = getSimpleValue(parametersNode, "muId", RpcStrType);

	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("state","string"));
	val = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

    if(val_muid.getType() == RpcVoid){//A类
        if(val.getType() == RpcVoid)
            return 0;
        call.addParam(val);
    }else{//B类
        call.addParam(val_muid);
        if(val.getType() == RpcVoid)
            return 0;
        call.addParam(val);
    }

	return 1;
}

int MethodCallParser::parseGetCamResState(DOMNode *parametersNode) {
	Value val[3];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("resId","string"));
	val[2] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 3; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}

/***************************************************************************
 *  AreaNode ---> CoreNode
 **************************************************************************/
 //B.2.5　SIP注册  区域节点向核心节点注册
int MethodCallParser::parseMURegister(DOMNode * parametersNode) {
	Value val[9];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "muIp", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "muPort", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "muName", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "muPassword", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "manufacturerld", RpcStrType);
	val[6] = getSimpleValue(parametersNode, "manufacturerName", RpcStrType);
	val[7] = getSimpleValue(parametersNode, "productVersion", RpcStrType);
	val[8] = getSimpleValue(parametersNode, "softwareVersion", RpcStrType);

	for(int i = 0; i < 9; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	return 1;
}
//B.2.2　目录资源上报 区域节点向核心节点上报目录资源
int MethodCallParser::parseResReport(DOMNode *parametersNode) {
	Value val[4];
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "totalPacketNum", RpcInteger);
	val[2] = getSimpleValue(parametersNode, "curPacketNum", RpcInteger);

	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));	
	fieldValueMap.insert(pair<string,string>("location","string"));
	fieldValueMap.insert(pair<string, string>("purpose","string"));
	fieldValueMap.insert(pair<string,string>("information","string"));
	val[3] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.3　目录资源变更
int MethodCallParser::parseResChange(DOMNode *parametersNode) {
	Value val[3];
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cmd", RpcStrType);

	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));	
	fieldValueMap.insert(pair<string,string>("location","string"));
	fieldValueMap.insert(pair<string, string>("purpose","string"));
	fieldValueMap.insert(pair<string,string>("information","string"));
	val[2] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 3; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.6　心跳	  区域节点向核心节点发送心跳信息
int MethodCallParser::parseMUKeepAlive(DOMNode *parametersNode) {

	Value val;
	if(!parametersNode) return 0;

	val = getSimpleValue(parametersNode, "muKeepAlivePeriod", RpcStrType);
	if(val.getType() == RpcVoid)
		return 0;

	call.addParam(val);
	return 1;
}
//B.2.15　摄像机状态上报 - 共用
//B.2.17　用户资源上报 UserResReport
int MethodCallParser::parseUserResReport(DOMNode *parametersNode) {
	Value val[5];
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "muName", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "totalPkt", RpcInteger);
	val[3] = getSimpleValue(parametersNode, "pktNum", RpcInteger);
	
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));	
	val[4] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 5; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.19　用户资源变更 UserResChange
int MethodCallParser::parseUserResChange(DOMNode *parametersNode) {
	Value val[5];
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "totalPkt", RpcInteger);
	val[2] = getSimpleValue(parametersNode, "pktNum", RpcInteger);
	val[3] = getSimpleValue(parametersNode, "cmd", RpcInteger);

	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));	
	val[4] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 5; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.21　告警资源上报
int MethodCallParser::parseAlarmResListReport(DOMNode *parametersNode) {
	Value val[5];
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "muName", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "totalPkt", RpcInteger);
	val[3] = getSimpleValue(parametersNode, "pktNum", RpcInteger);
	
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));	
	fieldValueMap.insert(pair<string,string>("description","string"));	
	val[4] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 5; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.22　告警资源变更
int MethodCallParser::parseAlarmResListChange(DOMNode *parametersNode) {
	Value val[7];
	unordered_map<string,string> fieldValueMap;

	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "muName", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "num", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "totalPkt", RpcInteger);
	val[4] = getSimpleValue(parametersNode, "pktNum", RpcInteger);
	val[5] = getSimpleValue(parametersNode, "cmd", RpcInteger);

	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));	
	fieldValueMap.insert(pair<string,string>("type","string"));	
	val[6] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 7; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.24　告警信息上报——与A类共用


/***************************************************************************
 *  CoreNode ---> AreaNode
 **************************************************************************/
//B.2.4　历史视频查询 核心节点向区域节点查询
int MethodCallParser::parseQueryHistoryFiles(DOMNode *parametersNode) {
	Value val[6];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "cuId", RpcInteger);
	val[4] = getSimpleValue(parametersNode, "fromDate", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "toDate", RpcInteger);
	
	for(int i = 0; i < 6; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.7　请求实时媒体流
int MethodCallParser::parseStartMediaReq(DOMNode *parametersNode) {
	Value val[8];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "mediaType", RpcInteger);
	val[4] = getSimpleValue(parametersNode, "linkMode", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "targetIpAddr", RpcInteger);
	val[6] = getSimpleValue(parametersNode, "targetPort", RpcStrType);
	val[7] = getSimpleValue(parametersNode, "flag", RpcInteger);
	
	for(int i = 0; i < 8; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.8　实时媒体流传输（TCP连接方式）  核心节点DDU向区域节点DDU 
int MethodCallParser::parseINFO(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.9　停止视频传输 
int MethodCallParser::parseStopMediaReq(DOMNode *parametersNode) {
	Value val[3];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "stopFlag", RpcInteger);
	
	for(int i = 0; i < 3; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.10　请求历史视频 
int MethodCallParser::parseStartPlayBack(DOMNode *parametersNode) {
	Value val[10];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "startTime", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "endTime", RpcStrType);
	
	val[5] = getSimpleValue(parametersNode, "linkMode", RpcInteger);
	val[6] = getSimpleValue(parametersNode, "targetIpAddr", RpcStrType);
	val[7] = getSimpleValue(parametersNode, "targetPort", RpcStrType);
	val[8] = getSimpleValue(parametersNode, "flag", RpcInteger);
	val[9] = getSimpleValue(parametersNode, "locationFlag", RpcInteger);
	
	for(int i = 0; i < 10; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.11　历史视频媒体流传输（TCP连接方式） 核心节点DDU向区域节点DDU
int MethodCallParser::parseHisInfo(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.12　历史视频播放控制
int MethodCallParser::parseControlFileBack(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "cmd", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "param", RpcInteger);
	
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.13　请求历史视频下载
int MethodCallParser::parseStartHisLoad(DOMNode *parametersNode) {
	Value val[10];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "startTime", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "endTime", RpcStrType);
	
	val[5] = getSimpleValue(parametersNode, "linkMode", RpcInteger);
	val[6] = getSimpleValue(parametersNode, "targetIpAddr", RpcStrType);
	val[7] = getSimpleValue(parametersNode, "targetPort", RpcStrType);
	val[8] = getSimpleValue(parametersNode, "flag", RpcInteger);
	val[9] = getSimpleValue(parametersNode, "locationFlag", RpcInteger);
	
	for(int i = 0; i < 10; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.14　历史视频下载传输（TCP连接方式） 核心节点DDU向区域节点DDU
int MethodCallParser::parseHisLoadInfo(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;
	
	val[0] = getSimpleValue(parametersNode, "sessionId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}
	
	return 1;
}
//B.2.16　摄像机状态查询 
int MethodCallParser::parseReqCamResState(DOMNode *parametersNode) {
	Value val[2];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);

	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("resId","string"));
	val[1] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 2; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.18　用户动态信息获取
int MethodCallParser::parseGetUserCurState(DOMNode *parametersNode) {
	Value val[2];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "curUserId", RpcStrType);

	for(int i = 0; i < 2; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.20　用户摄像机资源屏蔽
int MethodCallParser::parseSetUserCamManage(DOMNode *parametersNode) {
	Value val[8];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuLevel", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "action", RpcInteger);
	val[3] = getSimpleValue(parametersNode, "startTime", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "endTime", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "schduleCreatTime", RpcStrType);
	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("camId","string"));
	val[6] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);
	
	unordered_map<string, string> whiteUserFieldValueMap;
	whiteUserFieldValueMap.insert(pair<string,string>("id","string"));
	val[7] = getArrayValue(parametersNode, "whiteUser", "URL", whiteUserFieldValueMap);

	for(int i = 0; i < 8; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.23　告警信息订阅
int MethodCallParser::parseAlarmResSubscribe(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "muName", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "action", RpcInteger);

	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	val[3] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.25　订阅告警的信息查询 ———— 与A类共用
//B.2.26　告警信息查询
int MethodCallParser::parseReportAlarmInfo(DOMNode *parametersNode) {
	Value val[3];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "muName", RpcStrType);

	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	fieldValueMap.insert(pair<string,string>("startTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	val[2] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 3; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.27　云镜控制   
int MethodCallParser::parseControlPTZ(DOMNode *parametersNode) {
	Value val[6];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "resId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "userId", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "userLevel", RpcStrType);
	val[3] = getSimpleValue(parametersNode, "cmd", RpcStrType);
	val[4] = getSimpleValue(parametersNode, "param", RpcStrType);
	val[5] = getSimpleValue(parametersNode, "speed", RpcInteger);

	for(int i = 0; i < 6; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.28　跨节点资源下发
int MethodCallParser::parseResTransOrder(DOMNode *parametersNode) {
	Value val[4];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "totalPacketNum", RpcStrType);
	val[2] = getSimpleValue(parametersNode, "curPacketNum", RpcStrType);
	
	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));
	fieldValueMap.insert(pair<string,string>("location","string"));
	fieldValueMap.insert(pair<string,string>("purpose","string"));
	fieldValueMap.insert(pair<string,string>("information","string"));
	
	val[3] = getStructValue(parametersNode, "resources", fieldValueMap);

	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
//B.2.29　跨节点资源变更
int MethodCallParser::parseResChangeOrder(DOMNode *parametersNode) {
	Value val[3];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "muId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cmd", RpcStrType);
	
	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));
	fieldValueMap.insert(pair<string,string>("location","string"));
	fieldValueMap.insert(pair<string,string>("purpose","string"));
	fieldValueMap.insert(pair<string,string>("information","string"));
	
	val[2] = getStructValue(parametersNode, "resources", fieldValueMap);

	for(int i = 0; i < 3; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
	}

	return 1;
}
 /************************   end   ****************************************/

ULXR_API_IMPL(MethodCall)
  MethodCallParser::getMethodCall()
{
	return call;
}

}  // namespace ulxr
