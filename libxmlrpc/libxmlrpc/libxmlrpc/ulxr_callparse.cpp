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
				//cout << call.getMethodName() << " "<< cmdMap[call.getMethodName()]<<endl;
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
/*
Value MethodCallParser::getStructValue(DOMNode *parametersNode, string valueName, map<string,string> fieldValueMap) {
	DOMNodeList *childNodes = parametersNode->getChildNodes();
	DOMNode *node;
	CppString paramName;
	Struct structValue;
	Value val, fieldValue;
	ValueType type;
	
	for(int i = 0; i < childNodes->getLength(); i++) {
		node = childNodes->item(i);
		paramName = XMLString::transcode(node->getNodeName());
		if(paramName != valueName) continue;
		
		map<string,string>::iterator iter;
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
*/
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
		if(paramName != valueName || count != index) continue;
		else count ++;

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
	//cout << group.getXml() << endl;

	return Value(valueName, group);
}

//////////////////////////////////////////////////////////////////////////////////////

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
	fieldValueMap.insert(pair<string, string>("starTime","string"));
	fieldValueMap.insert(pair<string,string>("endTime","string"));
	val[3] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);
 
	for(int i = 0; i < 4; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;		
		call.addParam(val[i]);
	}
	
	return 1;
}

int MethodCallParser::parseReportAlarmRes(DOMNode *parametersNode) {
	Value val;
	if(!parametersNode) return 0;

	unordered_map<string,string> fieldValueMap;
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
	
	return 1;
}

int MethodCallParser::parseQueryAlarmRes(DOMNode *parametersNode) {
	Value val[3];
	if(!parametersNode) return 0;

	val[0] = getSimpleValue(parametersNode, "cuId", RpcStrType);
	val[1] = getSimpleValue(parametersNode, "cuUserId", RpcStrType);
	
	unordered_map<string,string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("id", "string"));
	fieldValueMap.insert(pair<string,string>("type", "string"));
	val[2] = getArrayValue(parametersNode, "group", "URL", fieldValueMap);

	for(int i = 0; i < 3; i++) {
		if(val[i].getType() == RpcVoid)
			return 0;
		call.addParam(val[i]);
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

int MethodCallParser::parseReportCamResState(DOMNode *parametersNode) {
	Value val;
	if(!parametersNode) return 0;
	
	unordered_map<string, string> fieldValueMap;
	fieldValueMap.insert(pair<string,string>("resId","string"));
	fieldValueMap.insert(pair<string,string>("state","string"));
	val = getArrayValue(parametersNode, "group", "URL", fieldValueMap);
	
	if(val.getType() == RpcVoid)
		return 0;
	call.addParam(val);
	
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

ULXR_API_IMPL(MethodCall)
  MethodCallParser::getMethodCall()
{
	return call;
}

}  // namespace ulxr
