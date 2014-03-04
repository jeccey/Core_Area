/**************************************************************************
               ulxr_responseparse.cpp  -  parse xml-rpc method call
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
#include <memory>
#include <iostream>

#define ULXR_NEED_EXPORTS
#include <ulxmlrpcpp.h>  // always first header

#include <ulxr_responseparse.h>
#include <ulxr_except.h>

using namespace xercesc;
using namespace std;

namespace ulxr {

MethodResponseParser::MethodResponseParser() {
	parser[0] = &MethodResponseParser::parseCuRegister;
	cmdMap.insert(pair<string,int>("CuRegister",0));

	parser[1] = &MethodResponseParser::parseCuKeepAlive;
	cmdMap.insert(pair<string,int>("CuKeepAlive",1));

	parser[2] = &MethodResponseParser::parseGetDepResList;
	cmdMap.insert(pair<string,int>("GetDepResList",2));

	parser[3] = &MethodResponseParser::parseGetVideoResList;
	cmdMap.insert(pair<string,int>("GetVideoResList",3));

	parser[4] = &MethodResponseParser::parseGetAlarmResList;
	cmdMap.insert(pair<string,int>("GetAlarmResList",4));

	parser[5] = &MethodResponseParser::parseQueryAlarmInfo;
	cmdMap.insert(pair<string,int>("QueryAlarmInfo",5));

	parser[6] = &MethodResponseParser::parseReportAlarmRes;
	cmdMap.insert(pair<string,int>("ReportAlarmRes",6));

	parser[7] = &MethodResponseParser::parseQueryAlarmRes;
	cmdMap.insert(pair<string,int>("QueryAlarmRes",7));

	parser[8] = &MethodResponseParser::parseAlarmHisQuery;
	cmdMap.insert(pair<string,int>("AlarmHisQuery",8));

	parser[9] = &MethodResponseParser::parsePTZCtrl;
	cmdMap.insert(pair<string,int>("PTZCtrl",9));

	parser[10] = &MethodResponseParser::parsePlayQuery;
	cmdMap.insert(pair<string,int>("PlayQuery",10));

	parser[11] = &MethodResponseParser::parsePlayOpen;
	cmdMap.insert(pair<string,int>("PlayOpen",11));

	parser[12] = &MethodResponseParser::parsePlayStart;
	cmdMap.insert(pair<string,int>("PlayStart",12));

	parser[13] = &MethodResponseParser::parseHisPlayOpen;
	cmdMap.insert(pair<string,int>("HisPlayOpen",13));

	parser[14] = &MethodResponseParser::parseHisPlayOpen;
	cmdMap.insert(pair<string,int>("HisPlayStart",14));

	parser[15] = &MethodResponseParser::parseHisLoadOpen;
	cmdMap.insert(pair<string,int>("HisLoadOpen",15));

	parser[16] = &MethodResponseParser::parseHisLoadStart;
	cmdMap.insert(pair<string,int>("HisLoadStart",16));

	parser[17] = &MethodResponseParser::parsePlayClose;
	cmdMap.insert(pair<string,int>("PlayClose",17));

	parser[18] = &MethodResponseParser::parsePlayClose;
	cmdMap.insert(pair<string,int>("PlayCtrl",18));

	parser[19] = &MethodResponseParser::parseReportCamResState;
	cmdMap.insert(pair<string,int>("ReportCamResState",19));

	parser[20] = &MethodResponseParser::parseReportCamResState;
	cmdMap.insert(pair<string,int>("GetCamResState",20));

	/*******************************************************************
     *  AreaNode ---> CoreNode
    ******************************************************************/
    parser[24] = &MethodResponseParser::parseMUKeepAlive;
    cmdMap.insert(pair<string, int>("MUKeepAlive", 24));
#if 1
    parser[21] = &MethodResponseParser::parseMURegister;
	cmdMap.insert(pair<string,int>("MURegister",21));

	parser[22] = &MethodResponseParser::parseResReport;
	cmdMap.insert(pair<string,int>("ResReport",22));

    parser[23] = &MethodResponseParser::parseResChange;
    cmdMap.insert(pair<string, int>("ResChange", 23));

    parser[24] = &MethodResponseParser::parseMUKeepAlive;
    cmdMap.insert(pair<string, int>("MUKeepAlive", 24));

    parser[25] = &MethodResponseParser::parseReportCamResState;
    cmdMap.insert(pair<string, int>("ReportCamResState", 25));

    parser[26] = &MethodResponseParser::parseUserResReport;
    cmdMap.insert(pair<string, int>("UserResReport", 26));

    parser[27] = &MethodResponseParser::parseUserResChange;
    cmdMap.insert(pair<string, int>("UserResChange", 27));

    parser[28] = &MethodResponseParser::parseAlarmResListReport;
    cmdMap.insert(pair<string, int>("AlarmResListReport", 28));

    parser[29] = &MethodResponseParser::parseAlarmResListChange;
    cmdMap.insert(pair<string, int>("AlarmResListChange", 29));

    parser[30] = &MethodResponseParser::parseReportAlarmRes;
    cmdMap.insert(pair<string, int>("ReportAlarmRes", 30));
    /*******************************************************************
     *  CoreNode ---> AreaNode
     ******************************************************************/
    parser[31] = &MethodResponseParser::parseQueryHistoryFiles;
    cmdMap.insert(pair<string, int>("QueryHistoryFiles", 31));

    parser[32] = &MethodResponseParser::parseStartMediaReq;
    cmdMap.insert(pair<string, int>("StartMediaReq", 32));

    parser[33] = &MethodResponseParser::parseINFO;
    cmdMap.insert(pair<string, int>("INFO", 33));

    parser[34] = &MethodResponseParser::parseStopMediaReq;
    cmdMap.insert(pair<string, int>("StopMediaReq", 34));

    parser[35] = &MethodResponseParser::parseStartPlayBack;
    cmdMap.insert(pair<string, int>("StartPlayBack", 35));

    parser[36] = &MethodResponseParser::parseHisInfo;
    cmdMap.insert(pair<string, int>("HisInfo", 36));

    parser[37] = &MethodResponseParser::parseControlFileBack;
    cmdMap.insert(pair<string, int>("ControlFileBack", 37));

    parser[38] = &MethodResponseParser::parseStartHisLoad;
    cmdMap.insert(pair<string, int>("StartHisLoad", 38));

    parser[39] = &MethodResponseParser::parseHisLoadInfo;
    cmdMap.insert(pair<string, int>("HisLoadInfo", 39));

    parser[40] = &MethodResponseParser::parseReqCamResState;
    cmdMap.insert(pair<string, int>("ReqCamResState", 40));

    parser[41] = &MethodResponseParser::parseGetUserCurState;
    cmdMap.insert(pair<string, int>("GetUserCurState", 41));

    parser[42] = &MethodResponseParser::parseSetUserCamManage;
    cmdMap.insert(pair<string, int>("SetUserCamManage", 42));

    parser[43] = &MethodResponseParser::parseAlarmResSubscribe;
    cmdMap.insert(pair<string, int>("AlarmResSubscribe", 43));

    //parser[44] = &MethodResponseParser::parseQueryAlarmRes_B;
    //cmdMap.insert(pair<string, int>("QueryAlarmRes_B", 44));
    parser[44] = &MethodResponseParser::parseQueryAlarmRes;
    cmdMap.insert(pair<string, int>("QueryAlarmRes", 44));

    parser[45] = &MethodResponseParser::parseReportAlarmInfo;
    cmdMap.insert(pair<string, int>("ReportAlarmInfo", 45));

    parser[46] = &MethodResponseParser::parseControlPTZ;
    cmdMap.insert(pair<string, int>("ControlPTZ", 46));

    parser[47] = &MethodResponseParser::parseResTransOrder;
    cmdMap.insert(pair<string, int>("ResTransOrder", 47));

    parser[48] = &MethodResponseParser::parseResChangeOrder;
    cmdMap.insert(pair<string, int>("ResChangeOrder", 48));
#endif
}

ULXR_API_IMPL(int) MethodResponseParser::parse(CppString buffer)
{
	int ret = 0;
	DOMNode *rootNode, *resultNode, *parametersNode;
    DOMNodeList *childNodes;

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
	DOMNodeList *nodeList = doc->getElementsByTagName(XMLString::transcode("response"));
	if(!nodeList || nodeList->getLength() > 1)
		goto end;
	rootNode = nodeList->item(0);
	childNodes = rootNode->getChildNodes();
	resultNode = childNodes->item(0);

	//获取命令名称
	if(rootNode->hasAttributes()) {
		DOMNamedNodeMap *nodeMap = rootNode->getAttributes();
		string nodeName, cmdName;
		for(int i = 0; i < nodeMap->getLength(); i++) {
			nodeName = XMLString::transcode(nodeMap->item(i)->getNodeName());
			if (nodeName == "command") {
				DOMNode *childNode = rootNode->getFirstChild();

				cmdName = XMLString::transcode(nodeMap->item(i)->getNodeValue());
				resp.setMethodName(cmdName);
				/*ret = (this->*parser[cmdMap[cmdName]])(childNode);*/
				break;
			}
		}
		if(cmdName.empty())
			goto end;
	} else
		goto end;

	if(resultNode->hasAttributes()) {
		DOMNamedNodeMap *nodeMap = resultNode->getAttributes();
		string nodeName, code;
		for(int i = 0; i < nodeMap->getLength(); i++) {
			nodeName = XMLString::transcode(nodeMap->item(i)->getNodeName());
			if (nodeName == "code") {
				DOMNode *childNode = rootNode->getFirstChild();
				code = XMLString::transcode(nodeMap->item(i)->getNodeValue());
				break;
			}
		}
		if(code.empty())
			goto end;
		if(code != "0") {
			Integer resultCode(code);
			string resultMessage = XMLString::transcode(resultNode->getFirstChild()->getNodeValue());
			resp.setFault(resultCode.getInteger(), resultMessage);
			ret = 1;
			goto end;
		} else {
			parametersNode = childNodes->item(1);
			//cout << resp.getMethodName() <<" " << cmdMap[resp.getMethodName()] << endl;
			ret = (this->*parser[cmdMap[resp.getMethodName()]])(parametersNode);
		}

	}

end:
	delete pMemBufInputSource;
	delete pDomParser;
	return ret;
}

Value MethodResponseParser::getSimpleValue(DOMNode *parametersNode, string valueName, ValueType type) {
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

Value MethodResponseParser::getStructValue(DOMNode *parametersNode, string valueName, unordered_map<string,string> fieldValueMap, int index) {
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
		if(count != index) {
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

Value MethodResponseParser::getArrayValue(DOMNode *parametersNode, string valueName, string itemName, unordered_map<string,string> itemFieldValueMap) {
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
	//cout << group.getXml() << endl;

	return Value(valueName, group);
}

//B类:订阅告警信息的查询 Group;  valueName=group
Value MethodResponseParser::getQueryAlarmRes(DOMNode *parametersNode, string valueName, string itemName, unordered_map<string,string> urlFieldValueMap){

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
        url.clear();
		url = getStructValue(item, "url", urlFieldValueMap);
		URL.addMember("url", Value("url", url));

		URL.addMember("alarmHisRecord", getSimpleValue(item, "alarmHisRecord", RpcStrType));
		URL.addMember("state", getSimpleValue(item, "state", RpcStrType));
		URL.addMember("time", getSimpleValue(item, "time", RpcStrType));
		URL.addMember("id", getSimpleValue(item, "id", RpcStrType));
		URL.addMember("type", getSimpleValue(item, "type", RpcStrType));
		URL.addMember("id", getSimpleValue(item, "id", RpcStrType));
/*
        URL.clear();
		URL.addMember("id", getSimpleValue(item, "id", RpcStrType));
		URL.addMember("type", getSimpleValue(item, "type", RpcStrType));
		URL.addMember("time", getSimpleValue(item, "time", RpcStrType));
		URL.addMember("state", getSimpleValue(item, "state", RpcStrType));
		URL.addMember("alarmHisRecord", getSimpleValue(item, "alarmHisRecord", RpcStrType));

        url.clear();
		url = getStructValue(item, "url", urlFieldValueMap);
		URL.addMember("url", Value("url", url));
*/
        //group添加
		group.addItem(Value("URL", URL));
	}

	return Value(valueName, group);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int MethodResponseParser::parseCuRegister(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("cuId",getSimpleValue(parametersNode,"cuId",RpcStrType));
	st.addMember("keepAlivePeriod",getSimpleValue(parametersNode,"keepAlivePeriod",RpcInteger));
	st.addMember("currentTime",getSimpleValue(parametersNode,"currentTime",RpcStrType));

	resp.setMethodName("CuRegister");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseCuKeepAlive(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("keepAlivePeriod",getSimpleValue(parametersNode,"keepAlivePeriod",RpcStrType));

	resp.setMethodName("keepAlivePeriod");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseGetDepResList(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("totalPkt",getSimpleValue(parametersNode,"totalPkt",RpcInteger));
	st.addMember("pktNum",getSimpleValue(parametersNode,"pktNum",RpcInteger));
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));
	fieldValueMap.insert(pair<string,string>("parentId","string"));
	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("GetDepResList");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseGetVideoResList(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("totalPkt",getSimpleValue(parametersNode,"totalPkt",RpcInteger));
	st.addMember("pktNum",getSimpleValue(parametersNode,"pktNum",RpcInteger));
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));
	fieldValueMap.insert(pair<string,string>("depId","string"));
	fieldValueMap.insert(pair<string,string>("pos","string"));
	fieldValueMap.insert(pair<string,string>("target","string"));
	fieldValueMap.insert(pair<string,string>("remark","string"));
	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("GetVideoResList");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseGetAlarmResList(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("totalPkt",getSimpleValue(parametersNode,"totalPkt",RpcInteger));
	st.addMember("pktNum",getSimpleValue(parametersNode,"pktNum",RpcInteger));
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));
	fieldValueMap.insert(pair<string,string>("description","string"));

	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("GetAlarmResList");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseQueryAlarmInfo(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct URL;

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	fieldValueMap.insert(pair<string,string>("startTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	fieldValueMap.insert(pair<string,string>("description","string"));
	Value val = getArrayValue(parametersNode, "group", "URL", fieldValueMap);
	URL.addMember("group",val);
	resp.setMethodName("QueryAlarmInfo");
	resp.setResult(URL);

	return 1;
}
//A类、B类共用——告警信息上报
int MethodResponseParser::parseReportAlarmRes(DOMNode *parametersNode) {
	resp.setMethodName("ReportAlarmRes");

	Struct st;

    if(!parametersNode){	//A类
		resp.setResult(Value());
	}else{	//B类
		st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));
		resp.setResult(st);
	}

	return 1;
}

//A类、B类共用——订阅告警的信息查询!!!!!
int MethodResponseParser::parseQueryAlarmRes(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	fieldValueMap.insert(pair<string,string>("state","string"));
	//直接解析
	CppString paramName;
	DOMNode *node, *URL;
	DOMNodeList *childNodes = parametersNode->getChildNodes();
	if(!childNodes) return 0;
	int i;
	for(i = 0; i < childNodes->getLength(); i++) {
		node = childNodes->item(i);
		paramName = XMLString::transcode(node->getNodeName());
		if(paramName == "group") break;
	}
	if(i >= childNodes->getLength()) return 0;
	DOMNodeList *items = node->getChildNodes();
	URL = items->item(0);
    DOMNodeList *url_list = URL->getChildNodes();

	if(url_list->getLength() == 3){	//A 类
		st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));
	}
	else{	//B类
        unordered_map<string,string> urlFieldValueMap;
		urlFieldValueMap.insert(pair<string,string>("resId","string"));
		urlFieldValueMap.insert(pair<string,string>("time","string"));

		st.addMember("group", getQueryAlarmRes(parametersNode, "group", "URL", urlFieldValueMap));
	}
	resp.setMethodName("QueryAlarmRes");
	resp.setResult(st);

	return 1;
}
#if 0
//B类接口 —— 订阅告警的信息查询!!!!!
int MethodResponseParser::parseQueryAlarmRes_B(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

    //直接解析
	CppString paramName;
	DOMNode *node, *URL;
	DOMNodeList *childNodes = parametersNode->getChildNodes();
	if(!childNodes) return 0;
	int i;
	for(i = 0; i < childNodes->getLength(); i++) {
		node = childNodes->item(i);
		paramName = XMLString::transcode(node->getNodeName());
		if(paramName == "group") break;
	}
	if(i >= childNodes->getLength()) return 0;
	DOMNodeList *items = node->getChildNodes();
	URL = items->item(0);
    DOMNodeList *url_list = URL->getChildNodes();

	unordered_map<string,string> urlFieldValueMap;
	urlFieldValueMap.insert(pair<string,string>("resId","string"));
	urlFieldValueMap.insert(pair<string,string>("time","string"));

	st.addMember("group", getQueryAlarmRes(parametersNode, "group", "URL", urlFieldValueMap));

    resp.setMethodName("QueryAlarmRes_B");
	resp.setResult(st);

	return 1;
}
#endif
int MethodResponseParser::parseAlarmHisQuery(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("totalNumber",getSimpleValue(parametersNode, "totalNumber", RpcInteger));
	st.addMember("currentNumber",getSimpleValue(parametersNode, "currentNumber", RpcInteger));

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("alarmId","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	fieldValueMap.insert(pair<string,string>("alarmStartTime","string"));
	fieldValueMap.insert(pair<string,string>("alarmEndTime","string"));
	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("startTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	fieldValueMap.insert(pair<string,string>("size","string"));

	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("AlarmHisQuery");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parsePTZCtrl(DOMNode *parametersNode){
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("resId",getSimpleValue(parametersNode, "resId", RpcStrType));

	resp.setMethodName("PTZCtrl");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parsePlayQuery(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("totalNumber",getSimpleValue(parametersNode, "totalNumber", RpcInteger));
	st.addMember("currentNumber",getSimpleValue(parametersNode, "currentNumber", RpcInteger));

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("startTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	fieldValueMap.insert(pair<string,string>("size","string"));
	
	st.addMember("recoderURL",getStructValue(parametersNode,"recoderURL",fieldValueMap));

	resp.setMethodName("PlayQuery");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parsePlayOpen(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("cuId",getSimpleValue(parametersNode, "cuId", RpcStrType));
	st.addMember("resId",getSimpleValue(parametersNode, "resId", RpcStrType));
	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	st.addMember("tcpIp",getSimpleValue(parametersNode,"tcpIp",RpcStrType));
	st.addMember("tcpPort",getSimpleValue(parametersNode,"tcpPort",RpcStrType));

	resp.setMethodName("PlayOpen");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parsePlayStart(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode, "sessionId", RpcStrType));

	resp.setMethodName("PlayStart");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseHisPlayOpen(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("cuId",getSimpleValue(parametersNode, "cuId", RpcStrType));
	st.addMember("resId",getSimpleValue(parametersNode, "resId", RpcStrType));
	st.addMember("sessionId",getSimpleValue(parametersNode, "sessionId", RpcStrType));
	st.addMember("tcpIp",getSimpleValue(parametersNode, "tcpIp", RpcStrType));
	st.addMember("tcpPort",getSimpleValue(parametersNode, "tcpPort", RpcStrType));

	resp.setMethodName("HisPlayOpen");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseHisPlayStart(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode, "sessionId", RpcStrType));

	resp.setMethodName("HisPlayStart");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseHisLoadOpen(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("cuId",getSimpleValue(parametersNode, "cuId", RpcStrType));
	st.addMember("resId",getSimpleValue(parametersNode, "resId", RpcStrType));
	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	st.addMember("tcpIp",getSimpleValue(parametersNode,"tcpIp",RpcStrType));
	st.addMember("tcpPort",getSimpleValue(parametersNode,"tcpPort",RpcStrType));

	resp.setMethodName("HisLoadOpen");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parseHisLoadStart(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode, "sessionId", RpcStrType));

	resp.setMethodName("HisLoadStart");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parsePlayClose(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode, "sessionId", RpcStrType));

	resp.setMethodName("PlayClose");
	resp.setResult(st);

	return 1;
}

int MethodResponseParser::parsePlayCtrl(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode, "sessionId", RpcStrType));

	resp.setMethodName("PlayCtrl");
	resp.setResult(st);

	return 1;
}
//A类、B类共用—— 摄像机状态上报
int MethodResponseParser::parseReportCamResState(DOMNode *parametersNode) {
	Struct st;
	Array group;

	if(!parametersNode){	//A类
		resp.setMethodName("ReportCamResState");
		resp.setResult(Value());
	}else{	//B类

		resp.setMethodName("ReportCamResState");
		unordered_map<string,string> fieldValueMap;
		fieldValueMap.insert(pair<string,string>("resId","string"));
		st.addMember("group",getArrayValue(parametersNode,"group", "URL", fieldValueMap));

		resp.setResult(st);
	}

	return 1;
}

int MethodResponseParser::parseGetCamResState(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	Array group;

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("state","string"));
	st.addMember("group",getArrayValue(parametersNode,"group", "URL", fieldValueMap));

	resp.setMethodName("GetCamResState");
	resp.setResult(st);

	return 1;
}

/***************************************************************************
 *  AreaNode ---> CoreNode
 **************************************************************************/
 //B.2.5　SIP注册  区域节点向核心节点注册
int MethodResponseParser::parseMURegister(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("muKeepAlivePeriod",getSimpleValue(parametersNode,"muKeepAlivePeriod",RpcStrType));

	resp.setMethodName("MURegister");
	resp.setResult(st);

	return 1;
}
//B.2.2　目录资源上报 区域节点向核心节点上报目录资源
int MethodResponseParser::parseResReport(DOMNode *parametersNode) {

	resp.setMethodName("ResReport");
	resp.setResult(Value());

	return 1;
}
//B.2.3　目录资源变更 ResChange
int MethodResponseParser::parseResChange(DOMNode *parametersNode) {

	resp.setMethodName("ResChange");
	resp.setResult(Value());

	return 1;
}

//B.2.6　心跳	  区域节点向核心节点发送心跳信息
int MethodResponseParser::parseMUKeepAlive(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("muKeepAlivePeriod",getSimpleValue(parametersNode,"muKeepAlivePeriod",RpcStrType));
	
	resp.setMethodName("MUKeepAlive");
	resp.setResult(st);

	return 1;
}
//B.2.15　摄像机状态上报——与A类共用
//B.2.17　用户资源上报 UserResReport
int MethodResponseParser::parseUserResReport(DOMNode *parametersNode) {

	resp.setMethodName("UserResReport");
	resp.setResult(Value());

	return 1;
}
//B.2.19　用户资源变更
int MethodResponseParser::parseUserResChange(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));

	resp.setMethodName("UserResChange");
	resp.setResult(st);

	return 1;
}
//B.2.21　告警资源上报
int MethodResponseParser::parseAlarmResListReport(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));

	resp.setMethodName("AlarmResListReport");
	resp.setResult(st);

	return 1;
}
//B.2.22　告警资源变更
int MethodResponseParser::parseAlarmResListChange(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));

	resp.setMethodName("AlarmResListChange");
	resp.setResult(st);

	return 1;
}
//B.2.24　告警信息上报——与A类共用

/***************************************************************************
 *  CoreNode ---> AreaNode
 **************************************************************************/
 //B.2.4　历史视频查询 核心节点向区域节点查询
int MethodResponseParser::parseQueryHistoryFiles(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	
	st.addMember("resId",getSimpleValue(parametersNode,"resId",RpcStrType));
	st.addMember("cuId",getSimpleValue(parametersNode,"cuId",RpcStrType));
	st.addMember("totalNumber",getSimpleValue(parametersNode,"totalNumber",RpcInteger));
	st.addMember("currentNum",getSimpleValue(parametersNode,"currentNum",RpcInteger));
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("startTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	fieldValueMap.insert(pair<string,string>("size","string"));
	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("QueryHistoryFiles");
	resp.setResult(st);

	return 1;
} 
//B.2.7　请求实时媒体流
 int MethodResponseParser::parseStartMediaReq(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	st.addMember("tcpIp",getSimpleValue(parametersNode,"tcpIp",RpcStrType));
	st.addMember("tcpPort",getSimpleValue(parametersNode,"tcpPort",RpcStrType));

	resp.setMethodName("StartMediaReq");
	resp.setResult(st);

	return 1;
}
//B.2.8　实时媒体流传输（TCP连接方式）  核心节点DDU向区域节点DDU 
int MethodResponseParser::parseINFO(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	resp.setMethodName("INFO");
	resp.setResult(st);

	return 1;
}
//B.2.9　停止视频传输
int MethodResponseParser::parseStopMediaReq(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	resp.setMethodName("StopMediaReq");
	resp.setResult(st);

	return 1;
}
//B.2.10　请求历史视频
 int MethodResponseParser::parseStartPlayBack(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	st.addMember("tcpIp",getSimpleValue(parametersNode,"tcpIp",RpcStrType));
	st.addMember("tcpPort",getSimpleValue(parametersNode,"tcpPort",RpcStrType));

	resp.setMethodName("StartPlayBack");
	resp.setResult(st);

	return 1;
}
//B.2.11　历史视频媒体流传输（TCP连接方式） 核心节点DDU向区域节点DDU
int MethodResponseParser::parseHisInfo(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	resp.setMethodName("HisInfo");
	resp.setResult(st);

	return 1;
}
//B.2.12　历史视频播放控制  
int MethodResponseParser::parseControlFileBack(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	resp.setMethodName("ControlFileBack");
	resp.setResult(st);

	return 1;
}
//B.2.13　请求历史视频下载
 int MethodResponseParser::parseStartHisLoad(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	st.addMember("tcpIp",getSimpleValue(parametersNode,"tcpIp",RpcStrType));
	st.addMember("tcpPort",getSimpleValue(parametersNode,"tcpPort",RpcStrType));

	resp.setMethodName("StartHisLoad");
	resp.setResult(st);

	return 1;
}
//B.2.14　历史视频下载传输（TCP连接方式） 核心节点DDU向区域节点DDU
int MethodResponseParser::parseHisLoadInfo(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	st.addMember("sessionId",getSimpleValue(parametersNode,"sessionId",RpcStrType));
	resp.setMethodName("HisLoadInfo");
	resp.setResult(st);

	return 1;
}
//B.2.16　摄像机状态查询
int MethodResponseParser::parseReqCamResState(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("state","string"));
	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("ReqCamResState");
	resp.setResult(st);

	return 1;
} 
//B.2.18　用户动态信息获取
int MethodResponseParser::parseGetUserCurState(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	
	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));
	st.addMember("curUserId",getSimpleValue(parametersNode,"curUserId",RpcStrType));
	st.addMember("userIp",getSimpleValue(parametersNode,"userIp",RpcStrType));
	st.addMember("userState",getSimpleValue(parametersNode,"userState",RpcInteger));
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("name","string"));
	fieldValueMap.insert(pair<string,string>("size","string"));
	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("GetUserCurState");
	resp.setResult(st);

	return 1;
}

//B.2.20　用户摄像机资源屏蔽 
int MethodResponseParser::parseSetUserCamManage(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	st.addMember("cuId",getSimpleValue(parametersNode,"cuId",RpcStrType));
	resp.setMethodName("SetUserCamManage");
	resp.setResult(st);

	return 1;
}
//B.2.23　告警信息订阅
int MethodResponseParser::parseAlarmResSubscribe(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));
	resp.setMethodName("AlarmResSubscribe");
	resp.setResult(st);

	return 1;
}
//B.2.25　订阅告警的信息查询 ———— 与A类共用
//B.2.26　告警信息查询
int MethodResponseParser::parseReportAlarmInfo(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	
	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	fieldValueMap.insert(pair<string,string>("startTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	fieldValueMap.insert(pair<string,string>("message","string"));
	
	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("ReportAlarmInfo");
	resp.setResult(st);

	return 1;
}
//B.2.27　云镜控制
int MethodResponseParser::parseControlPTZ(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	
	st.addMember("resId",getSimpleValue(parametersNode,"resId",RpcStrType));

	resp.setMethodName("ControlPTZ");
	resp.setResult(st);

	return 1;
}
//B.2.28　跨节点资源下发
int MethodResponseParser::parseResTransOrder(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	
	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));

	resp.setMethodName("ResTransOrder");
	resp.setResult(st);

	return 1;
}
//B.2.29　跨节点资源变更
int MethodResponseParser::parseResChangeOrder(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;
	
	st.addMember("muId",getSimpleValue(parametersNode,"muId",RpcStrType));

	resp.setMethodName("ResChangeOrder");
	resp.setResult(st);

	return 1;
}
/******************************end******************************************/
ULXR_API_IMPL(MethodResponse)
  MethodResponseParser::getMethodResponse()
{
  return resp;
}

}  // namespace ulxr
