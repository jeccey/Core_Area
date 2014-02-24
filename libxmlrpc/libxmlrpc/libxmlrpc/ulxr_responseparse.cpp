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

	//»ñÈ¡ÃüÁîÃû³Æ
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

int MethodResponseParser::parseReportAlarmRes(DOMNode *parametersNode) {

	resp.setResult(Value());

	return 1;
}

int MethodResponseParser::parseQueryAlarmRes(DOMNode *parametersNode) {
	if(!parametersNode) return 0;
	Struct st;

	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id","string"));
	fieldValueMap.insert(pair<string,string>("type","string"));
	fieldValueMap.insert(pair<string,string>("state","string"));
	
	st.addMember("group",getArrayValue(parametersNode,"group","URL",fieldValueMap));

	resp.setMethodName("QueryAlarmRes");
	resp.setResult(st);

	return 1;
}

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

int MethodResponseParser::parseReportCamResState(DOMNode *parametersNode) {
	//if(!parametersNode) return 0;
	
	resp.setMethodName("ReportCamResState");
	resp.setResult(Value());
	cout << "parseReportCamResState" << endl;
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

ULXR_API_IMPL(MethodResponse)
  MethodResponseParser::getMethodResponse()
{
  return resp;
}

}  // namespace ulxr
