/***************************************************************************
               ulxr_callparse.h  -  parse xml-rpc method call
                             -------------------
    begin                : Wed Mar 13 2002
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: ulxr_callparse.h 940 2006-12-30 18:22:05Z ewald-arnold $

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

#ifndef ULXR_CALLPARSE_H
#define ULXR_CALLPARSE_H

#include <ulxmlrpcpp.h>  // always first header
#include <ulxr_call.h>
#include <string>
#include <map>

//xerces headers
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOMDocument.hpp>

using namespace xercesc;
using namespace std;

namespace ulxr {

/** An xml parser for a MethodCall.
  * @ingroup grp_ulxr_parser
  */
class ULXR_API_DECL0 MethodCallParser
{
public:
	MethodCallParser();
	MethodCall getMethodCall();
	int parse(CppString buffer);
private:
	Value getSimpleValue(DOMNode *parametersNode, string valueName, ValueType type);
	Value getStructValue(DOMNode *parametersNode, string valueName, unordered_map<string,string> fieldValueMap, int index = 0);
	Value getArrayValue(DOMNode *parametersNode, string valueName, string itemName, unordered_map<string,string> itemFieldValueMap);
	Value getReportAlarmRes(DOMNode *parametersNode, string valueName, string itemName, unordered_map<string,string> itemFieldValueMap);//特殊处理
	int parseCuRegister(DOMNode *parametersNode);
	int parseCuKeepAlive(DOMNode *parametersNode);
	int parseGetDepResList(DOMNode *parametersNode);
	int parseGetVideoResList(DOMNode *parametersNode);
	int parseGetAlarmResList(DOMNode *parametersNode);
	int parseQueryAlarmInfo(DOMNode *parametersNode);
	int parseReportAlarmRes(DOMNode *parametersNode);       //与B类接口共用
	int parseQueryAlarmRes(DOMNode *parametersNode);        //与B类接口共用
	int parseAlarmHisQuery(DOMNode *parametersNode);
	int parsePTZCtrl(DOMNode *parametersNode);
	int parsePlayQuery(DOMNode *parametersNode);
	int parsePlayOpen(DOMNode *parametersNode);
	int parsePlayStart(DOMNode *parametersNode);
	int parseHisPlayOpen(DOMNode *parametersNode);
	int parseHisPlayStart(DOMNode *parametersNode);
	int parseHisLoadOpen(DOMNode *parametersNode);
	int parseHisLoadStart(DOMNode *parametersNode);
	int parsePlayClose(DOMNode *parametersNode);
	int parsePlayCtrl(DOMNode *parametersNode);
	int parseReportCamResState(DOMNode *parametersNode);    //与B类接口共用
	int parseGetCamResState(DOMNode *parametersNode);

    /********************************************************************
     *      解析请求参数 （AreaNode ---> CoreNode）
     *******************************************************************/
	int parseMURegister(DOMNode *parametersNode);
	int parseResReport(DOMNode *parametersNode);
    int parseResChange(DOMNode *parametersNode);
    int parseMUKeepAlive(DOMNode *parametersNode);
    int parseUserResReport(DOMNode *parametersNode);
    int parseUserResChange(DOMNode *parametersNode);
    int parseAlarmResListReport(DOMNode *parametersNode);
    int parseAlarmResListChange(DOMNode *parametersNode);
    //int parseReportCamResState(DOMNode *parametersNode);    //与A类接口共用
    //int parseReportAlarmRes(DOMNode *parametersNode);       //与A类接口共用

    /********************************************************************
     *      解析请求参数 （CoreNode ---> AreaNode）
     *******************************************************************/
    int parseQueryHistoryFiles(DOMNode *parametersNode);
    int parseStartMediaReq(DOMNode *parametersNode);
    int parseINFO(DOMNode *parametersNode);
    int parseStopMediaReq(DOMNode *parametersNode);
    int parseStartPlayBack(DOMNode *parametersNode);
    int parseHisInfo(DOMNode *parametersNode);
    int parseControlFileBack(DOMNode *parametersNode);
    int parseStartHisLoad(DOMNode *parametersNode);
    int parseHisLoadInfo(DOMNode *parametersNode);
    int parseReqCamResState(DOMNode *parametersNode);
    int parseGetUserCurState(DOMNode *parametersNode);
    int parseSetUserCamManage(DOMNode *parametersNode);
    int parseAlarmResSubscribe(DOMNode *parametersNode);
    //int parseQueryAlarmRes(DOMNode *parametersNode);        //与A类接口共用
    int parseReportAlarmInfo(DOMNode *parametersNode);
    int parseControlPTZ(DOMNode *parametersNode);
    int parseResTransOrder(DOMNode *parametersNode);
    int parseResChangeOrder(DOMNode *parametersNode);


private:
	MethodCall call;
	int (MethodCallParser::*parser[50])(DOMNode *parametersNodes);
	map<string,int> cmdMap;
};


}  // namespace ulxr


#endif // ULXR_CALLPARSE_H
