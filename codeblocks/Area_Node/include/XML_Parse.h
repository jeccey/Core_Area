#ifndef XML_PARSE_H
#define XML_PARSE_H

#include <string>
#include <string.h>
#include <iostream>
#include <map>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "Parameters.h"

#include "Area_callback.h"

/*功能类, 提供回调函数实现，调用对象（根据不同 command 解析 xml 的 Callback函数）*/
class XMLMessageParse
{
public:
    XMLMessageParse()
    {}
    ~XMLMessageParse()
    {}
    xmlDocPtr create_XML_reply(const char *command)
    {

        xmlDocPtr doc = NULL;            /* document pointer */
        xmlNodePtr root_node = NULL;     /* node pointer*/

        //1. Creates a new document, a node and set it as a root node
        doc = xmlNewDoc(BAD_CAST "1.0");
        doc->encoding = BAD_CAST strdup("GB2312");
        doc->standalone = 1;	//standalone = yes

        root_node = xmlNewNode(NULL, BAD_CAST "response");// request or response
        xmlDocSetRootElement(doc, root_node);   /* set document's root node */
        xmlNewProp(root_node, BAD_CAST "command", BAD_CAST command);

        return doc;
    }

    /************************* C.2.4 历史视频查询——xml解析  ***********************************/
    char* QueryHistoryFiles(char *xmlbody, int len)
    {
        xmlDocPtr doc;
        xmlNodePtr request, node;
        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令
        struct reqHistoryFile req_hf;	/**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /************** 修改下面的内容提取参数 到struct reqHistoryFile req_hf******************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定, 遍历 parameters 的子节点
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"resId")==0)          //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_hf.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"userId")==0)    //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_hf.userId, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"userLevel")==0)    //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_hf.userLevel, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"cuId")==0)    //get cuId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_hf.cuId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"fromDate")==0)    //get fromDate node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_hf.fromDate, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"toDate")==0)    //get toDate node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_hf.toDate, content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2----------------- 通过 回调函数获得 返回值 ----------------------------------------------------------------------------------
        struct respHistoryFile result_arg;                                                /*** 返回 值 *****************/
        result_arg = pcb_FunMap.QueryHistoryFiles(req_hf);          /*** 调用回调函数构造 返回值 ***/

//step 3--------------------------------------------------------------------------------------------------
        // 构造响应 xml_body
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
        xmlNodePtr parameters_node;
        char int_to_str[10];            //辅助将 int 转换 为 字符串

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muKeepAlivePeriod",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);

        //继续添加。。。。。。
        xmlNodePtr group;
        group = xmlNewNode (NULL, BAD_CAST "group");
        xmlAddChild (parameters_node, group);
        //先创建，后添加方式
        xmlNodePtr URL;

        int num;
        for(num = 0; num<result_arg.group_num; num++)
        {
            URL = NULL;
            URL = xmlNewNode (NULL, BAD_CAST "URL");
            xmlNewChild (URL, NULL, BAD_CAST "startTime",  BAD_CAST  result_arg.group[num].startTime );
            xmlNewChild (URL, NULL, BAD_CAST "endTime",  BAD_CAST  result_arg.group[num].endTime );
            xmlNewChild (URL, NULL, BAD_CAST "size",  BAD_CAST  result_arg.group[num].size );
            // 添加子节点 parameters_node 到 root 节点
            xmlAddChild (group, URL);
        }
        /***************  修改完毕    ************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests
        return (char *)xml_body;
    } // QueryHistoryFiles

/************************* 2.7 请求实时媒体流 —— xml解析 ********************************/
    char* StartMediaReq(char *xmlbody, int len)
    {
        struct StartMediaReq startMediaReq_args;	//~~请求参数,提取到该结构体中

        xmlDocPtr doc;
        xmlNodePtr request, node;

        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /*~~~~~~~~~~~~~~~~ 修改下面的内容提取参数 到struct StartMediaReq startMediaReq_args ~~~~~~~~~~~~~*/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定, 遍历 parameters 的子节点
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"resId")==0)          //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(startMediaReq_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"userId")==0)    //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(startMediaReq_args.userId, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"userLevel")==0)    //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(startMediaReq_args.userLevel, content);
                xmlFree(value);
            }
			else if (xmlStrcasecmp(node->name, BAD_CAST"mediaType")==0)    //get mediaType node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				startMediaReq_args.mediaType = atoi(content);
                xmlFree(value);
            }
			else if (xmlStrcasecmp(node->name, BAD_CAST"linkMode")==0)    //get linkMode node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				startMediaReq_args.linkMode = atoi(content);
                xmlFree(value);
            }
  			else if (xmlStrcasecmp(node->name, BAD_CAST"targetIpAddr")==0)    //get targetIpAddr node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(startMediaReq_args.targetIpAddr, content);
                xmlFree(value);
            }
			else if (xmlStrcasecmp(node->name, BAD_CAST"targetPort")==0)    //get targetPort node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(startMediaReq_args.targetPort, content);
                xmlFree(value);
            }
			else if (xmlStrcasecmp(node->name, BAD_CAST"flag")==0)    //get flag node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				startMediaReq_args.flag = atoi(content);
                xmlFree(value);
            }
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2--------通过 回调函数获得 返回值
        struct result_StartMediaReq result_StartMediaReq;              /*** 返回 值 *****************/
        result_StartMediaReq = pcb_FunMap.StartMediaReq(startMediaReq_args);    /*** 调用回调函数构造 返回值 ***/

//step 3--------------   修改     ---------------------------------------------------
        // 构造响应 填充好部分转换为sip_msg信息结构体
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;				//~~~~~~~~~~~~~~~~~
		xmlNodePtr parameters_node;			//~~~~~~~~~~~~~~~~~
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_StartMediaReq.value);
        sprintf(int_to_str,"%d",result_StartMediaReq.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

		//添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_StartMediaReq.sessionId );
		xmlNewChild (parameters_node, NULL, BAD_CAST "tcpIp",  BAD_CAST result_StartMediaReq.tcpIp );
		xmlNewChild (parameters_node, NULL, BAD_CAST "tcpPort",  BAD_CAST result_StartMediaReq.tcpPort );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);


        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } // StartMediaReq

/************************* C.2.9 停止实时媒体流传输 —— xml解析 ********************************/
    char* StopMediaReq(char *xmlbody, int len)
    {
        struct StopMediaReq stopMediaReq_args;	//~~请求参数,提取到该结构体中

        xmlDocPtr doc;
        xmlNodePtr request, node;
        //xmlNodePtr resources;						//~~

        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /*~~~~~~~~~~~~~~~~ 修改下面的内容提取参数 到struct StopMediaReq stopMediaReq_args ~~~~~~~~~~~~~*/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定, 遍历 parameters 的子节点
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"sessionId")==0)          //get sessionId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(stopMediaReq_args.sessionId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"resId")==0)    //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(stopMediaReq_args.resId, content);
                xmlFree(value);
            }
			else if (xmlStrcasecmp(node->name, BAD_CAST"stopFlag")==0)    //get stopFlag node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				stopMediaReq_args.stopFlag = atoi(content);
                xmlFree(value);
            }
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2--------通过 回调函数获得 返回值
        struct result result_stopMediaReq;              /*** 返回 值 *****************/
        result_stopMediaReq = pcb_FunMap.StopMediaReq(stopMediaReq_args);    /*** 调用回调函数构造 返回值 ***/
//step 3--------------   修改     ---------------------------------------------------
        // 构造响应 填充好部分转换为sip_msg信息结构体
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;				//~~~~~~~~~~~~~~~~~
		xmlNodePtr parameters_node;			//~~~~~~~~~~~~~~~~~
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_stopMediaReq.value);
        sprintf(int_to_str,"%d",result_stopMediaReq.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

		//添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_stopMediaReq.args );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);


        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } // StopMediaReq

/************************* C.2.10 请求历史视频回放 —— xml解析 ********************************/
    char* StartPlayBack(char *xmlbody, int len)
    {
        struct reqStartPlayBack req_args;	//~~～请求参数,提取到该结构体中

        xmlDocPtr doc;
        xmlNodePtr request, node;
        //xmlNodePtr resources;						//~~～

        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /*~~~~~~~~~~~~~~~~ 修改下面的内容提取参数 到   struct reqStartPlayBack req_args ~~~~~~~~~~~~~*/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定, 遍历 parameters 的子节点
        {
            if(xmlStrcasecmp(node->name, BAD_CAST"resId")==0)    //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"userId")==0)    //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"userLevel")==0)    //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userLevel, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"startTime")==0)    //get startTime node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.startTime, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"endTime")==0)    //get endTime node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.endTime, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"linkMode")==0)    //get linkMode node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.linkMode = atoi(content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"targetIpAddr")==0)    //get targetIpAddr node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.targetIpAddr, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"targetPort")==0)    //get targetPort node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.targetPort, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"flag")==0)    //get flag node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.flag = atoi(content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"locationFlag")==0)    //get locationFlag node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.locationFlag = atoi(content);
                xmlFree(value);
            }
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2--------通过 回调函数获得 返回值
        struct result_StartPlayBack result_args;              /*** 返回 值 *****************/
        result_args = pcb_FunMap.StartPlayBack(req_args);    /*** 调用回调函数构造 返回值 ***/
//step 3--------------   修改     ---------------------------------------------------
        // 构造响应 填充好部分转换为sip_msg信息结构体
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;				//~~~~~~~~~~~~~~~~~
		xmlNodePtr parameters_node;			//~~~~~~~~~~~~~~~~~
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_args.result_value);
        sprintf(int_to_str,"%d",result_args.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

		//添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_args.sessionId );
        xmlNewChild (parameters_node, NULL, BAD_CAST "tcpIp",  BAD_CAST result_args.tcpIp );
        xmlNewChild (parameters_node, NULL, BAD_CAST "tcpPort",  BAD_CAST result_args.tcpPort );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);


        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } // StopMediaReq

/************************* C.2.12 历史视频播放控制 —— xml解析 ********************************/
    char* ControlFileBack(char *xmlbody, int len)
    {
        struct req_ControlFileBack req_args;	//~~～请求参数,提取到该结构体中

        xmlDocPtr doc;
        xmlNodePtr request, node;
        //xmlNodePtr resources;						//~~～

        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /*~~~~~~~~~~~~~~~~ 修改下面的内容提取参数 到   struct reqStartPlayBack req_args ~~~~~~~~~~~~~*/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定, 遍历 parameters 的子节点
        {
            if(xmlStrcasecmp(node->name, BAD_CAST"sessionId")==0)    //get sessionId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.sessionId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"resId")==0)    //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"cmd")==0)    //get cmd node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.cmd, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"param")==0)    //get param node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.param = atoi(content);
                xmlFree(value);
            }
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2--------通过 回调函数获得 返回值
        struct result_ControlFileBack result_args;              /*** 返回 值 *****************/
        result_args = pcb_FunMap.ControlFileBack(req_args);    /*** 调用回调函数构造 返回值 ***/
//step 3--------------   修改     ---------------------------------------------------
        // 构造响应 填充好部分转换为sip_msg信息结构体
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;				//~~~~~~~~~~~~~~~~~
		xmlNodePtr parameters_node;			//~~~~~~~~~~~~~~~~~
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_args.result_value);
        sprintf(int_to_str,"%d",result_args.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

		//添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_args.sessionId );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } // ControlFileBack

/************************* C.2.13 请求历史视频下载 —— xml解析 ********************************/
    char* StartHisLoad(char *xmlbody, int len)
    {
        struct reqStartPlayBack req_args;	//~~～请求参数,提取到该结构体中

        xmlDocPtr doc;
        xmlNodePtr request, node;
        //xmlNodePtr resources;						//~~～

        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /*~~~~~~~~~~~~~~~~ 修改下面的内容提取参数 到   struct reqStartPlayBack req_args ~~~~~~~~~~~~~*/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定, 遍历 parameters 的子节点
        {
            if(xmlStrcasecmp(node->name, BAD_CAST"resId")==0)    //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"userId")==0)    //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"userLevel")==0)    //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userLevel, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"startTime")==0)    //get startTime node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.startTime, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"endTime")==0)    //get endTime node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.endTime, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"linkMode")==0)    //get linkMode node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.linkMode = atoi(content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"targetIpAddr")==0)    //get targetIpAddr node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.targetIpAddr, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name, BAD_CAST"targetPort")==0)    //get targetPort node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.targetPort, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"flag")==0)    //get flag node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.flag = atoi(content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"locationFlag")==0)    //get locationFlag node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.locationFlag = atoi(content);
                xmlFree(value);
            }
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2--------通过 回调函数获得 返回值
        struct result_StartPlayBack result_args;              /*** 返回 值 *****************/
        result_args = pcb_FunMap.StartPlayBack(req_args);    /*** 调用回调函数构造 返回值 ***/
//step 3--------------   修改     ---------------------------------------------------
        // 构造响应 填充好部分转换为sip_msg信息结构体
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;				//~~~~~~~~~~~~~~~~~
		xmlNodePtr parameters_node;			//~~~~~~~~~~~~~~~~~
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_args.result_value);
        sprintf(int_to_str,"%d",result_args.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

		//添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_args.sessionId );
        xmlNewChild (parameters_node, NULL, BAD_CAST "tcpIp",  BAD_CAST result_args.tcpIp );
        xmlNewChild (parameters_node, NULL, BAD_CAST "tcpPort",  BAD_CAST result_args.tcpPort );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);


        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }


/************************* C.2.16 摄像机资源上报——解析 ********************************/
    char* ReqCamResState(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct ReqCamResState req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct ResReportArgs req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0)          //get muId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0)     //get group node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0)
                    {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"resId")==0)   //get resId node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].resId, content);
                                xmlFree(value);
                            }
                            else if(xmlStrcasecmp(curl->name,BAD_CAST"state")==0)   //get state node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].state, content);
                                xmlFree(value);
                            }
                        i++;
                    }
                }//end traverse gurl node
            }
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2 --------通过 回调函数获得 返回值， 参数 （ struct ResReportArgs req_args
        struct result_ReqCamResState result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.ReqCamResState(req_args);    /*** 调用回调函数构造 返回值 ***/

//step 3--------------------------------------------------------------------------------------------------
        // 构造响应 xml_body
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);
        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.result_value);
        sprintf(int_to_str,"%d",result_arg.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);

           //继续添加。。。。。。
        xmlNodePtr group;
        group = xmlNewNode (NULL, BAD_CAST "group");
        xmlAddChild (parameters_node, group);

        //先创建，后添加方式
        xmlNodePtr URL;

        int num;
        for(num = 0; num<result_arg.urlNum; num++)
        {
            URL = NULL;
            URL = xmlNewNode (NULL, BAD_CAST "URL");
            xmlNewChild (URL, NULL, BAD_CAST "resId",  BAD_CAST  result_arg.group[num].resId );
            xmlNewChild (URL, NULL, BAD_CAST "state",  BAD_CAST  result_arg.group[num].state );
            // 添加子节点 parameters_node 到 root 节点
            xmlAddChild (group, URL);
        }


          /*************** 修改完成 ******************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }


/************************* c.2.18 用户动态信息获取 ——解析 ********************************/
    char* GetUserCurState(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node;
        xmlChar *value;
        char *command;
        char *content;
        struct UserCurState req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct UserCurState req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"MuId")==0)          //get MuId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"curUserId")==0)          //get curUserId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.curUserId, content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct UserCurState req_args) *********/
        struct result_UserCurState result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.GetUserCurState(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.result_value);
        sprintf(int_to_str,"%d",result_arg.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.muId );
        xmlNewChild (parameters_node, NULL, BAD_CAST "curUserId",  BAD_CAST result_arg.curUserId );
        xmlNewChild (parameters_node, NULL, BAD_CAST "userIp",  BAD_CAST result_arg.userIp );
        xmlNewChild (parameters_node, NULL, BAD_CAST "userState",  BAD_CAST result_arg.userState );

	// int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   result_arg.urlNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "urlNum",  (const xmlChar *) int_to_str);


        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);

           //继续添加。。。。。。
        xmlNodePtr group;
        group = xmlNewNode (NULL, BAD_CAST "group");
        xmlAddChild (parameters_node, group);

        //先创建，后添加方式
        xmlNodePtr URL;
        int num;
        for(num = 0; num<result_arg.urlNum; num++)
        {
            URL = NULL;
            URL = xmlNewNode (NULL, BAD_CAST "URL");
            xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  result_arg.group[num].id );
            xmlNewChild (URL, NULL, BAD_CAST "name",  BAD_CAST  result_arg.group[num].name );
            // 添加子节点 parameters_node 到 root 节点
            xmlAddChild (group, URL);
        }
          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* c.2.20 摄像机资源屏蔽 ——解析 ********************************/
    char* SetUserCamManage(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl,  curl;
        xmlChar *value;
        char *command;
        char *content;
        struct SetUserCamManage req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;
        int j = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct UserCurState req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"cuId")==0)          //get cuId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.cuId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"cuLevel")==0)          //get cuLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.cuLevel, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"Action")==0)    //get Action node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.Action = atoi(content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"startTime")==0)          //get startTime node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.startTime, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"endTime")==0)          //get endTime node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.endTime, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"schduleCreatTime")==0)          //get schduleCreatTime node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.schduleCreatTime, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0)     //get group node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0)
                    {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"camId")==0)   //get camId node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].camId, content);
                                xmlFree(value);
                            }
                        i++;
                    }
                }//end traverse gurl node
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"whiteUser") == 0)     //get whiteUser node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0)
                    {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0)   //get id node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group_id[j].id, content);
                                xmlFree(value);
                            }
                        j++;
                    }
                }//end traverse gurl node
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct UserCurState req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.SetUserCamManage(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "cuId",  BAD_CAST result_arg.args );

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* C.2.23 告警资源订阅- ——解析 ********************************/
    char* AlarmResSubscribe(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct AlarmResSubscribe req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct AlarmResSubscribe req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0)          //get muId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"muName")==0)          //get muName node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muName, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"action")==0)    //get action node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.action = atoi(content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0)     //get group node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0)
                    {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0)   //get id node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            }
                            else if(xmlStrcasecmp(curl->name,BAD_CAST"type")==0)   //get type node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].description, content);
                                xmlFree(value);
                            }
                        i++;
                    }
                }//end traverse gurl node
                req_args.urlNum = i;
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct UserCurState req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.AlarmResSubscribe(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* C.2.25 订阅告警的信息查询- ——解析 ********************************/
    char* QueryAlarmRes(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct QueryAlarmRes req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct QueryAlarmRes req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0)          //get muId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"muName")==0)          //get muName node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muName, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0)     //get group node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0)
                    {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0)   //get id node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            }
                            else if(xmlStrcasecmp(curl->name,BAD_CAST"type")==0)   //get type node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].type, content);
                                xmlFree(value);
                            }
                        i++;
                    }
                }//end traverse gurl node
                req_args.urlNum = i;
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct QueryAlarmRes req_args) *********/
        struct result_QueryAlarmRes result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.QueryAlarmRes(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.result_value);
        sprintf(int_to_str,"%d",result_arg.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.muId );
        xmlNewChild (parameters_node, NULL, BAD_CAST "muName",  BAD_CAST result_arg.muName );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

         //继续添加。。。。。。
        xmlNodePtr group;
        group = xmlNewNode (NULL, BAD_CAST "group");
        xmlAddChild (parameters_node, group);
        //先创建，后添加方式
        xmlNodePtr URL, url;
        int num;
        for(num = 0; num<result_arg.urlNum; num++)
        {
            URL = NULL;
            URL = xmlNewNode (NULL, BAD_CAST "URL");
            xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  result_arg.group[num].id );
            xmlNewChild (URL, NULL, BAD_CAST "type",  BAD_CAST  result_arg.group[num].type );
            xmlNewChild (URL, NULL, BAD_CAST "time",  BAD_CAST  result_arg.group[num].time );
            xmlNewChild (URL, NULL, BAD_CAST "state",  BAD_CAST  result_arg.group[num].state );
            	// int 需要转化
            memset(int_to_str, 0, sizeof(int_to_str));
            sprintf(int_to_str,"%d",  result_arg.group[num].alarmHisRecord );
            xmlNewChild (URL, NULL, BAD_CAST "alarmHisRecord",  (const xmlChar *) int_to_str);

            url = NULL;
            url = xmlNewNode (NULL, BAD_CAST "url");
            xmlNewChild (url, NULL, BAD_CAST "resId",  BAD_CAST  result_arg.group[num].rtUrl.resId );
            xmlNewChild (url, NULL, BAD_CAST "time",  BAD_CAST  result_arg.group[num].rtUrl.time );
            xmlAddChild (URL, url);

            xmlAddChild (group, URL);
        }

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }


/************************* C.2.26 告警信息查询- ——解析 ********************************/
    char* ReportAlarmInfo(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct ReportAlarmInfo req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct QueryAlarmRes req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0)          //get muId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"muName")==0)          //get muName node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muName, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0)     //get group node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0)
                    {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0)   //get id node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            }
                            else if(xmlStrcasecmp(curl->name,BAD_CAST"startTime")==0)   //get startTime node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].startTime, content);
                                xmlFree(value);
                            }
                            else if(xmlStrcasecmp(curl->name,BAD_CAST"endTime")==0)   //get endTime node
                            {
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].endTime, content);
                                xmlFree(value);
                            }
                        i++;
                    }
                }//end traverse gurl node
                req_args.urlNum = i;
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct ReportAlarmInfo req_args) *********/
        struct result_ReportAlarmInfo result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.ReportAlarmInfo(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.result_value);
        sprintf(int_to_str,"%d",result_arg.result_code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.muId );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

         //继续添加。。。。。。
        xmlNodePtr group;
        group = xmlNewNode (NULL, BAD_CAST "group");
        xmlAddChild (parameters_node, group);
        //先创建，后添加方式
        xmlNodePtr URL;
        int num;
        for(num = 0; num<result_arg.urlNum; num++)
        {
            URL = NULL;
            URL = xmlNewNode (NULL, BAD_CAST "URL");
            xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  result_arg.group[num].id );
            xmlNewChild (URL, NULL, BAD_CAST "startTime",  BAD_CAST  result_arg.group[num].startTime );
            xmlNewChild (URL, NULL, BAD_CAST "endTime",  BAD_CAST  result_arg.group[num].endTime );
            xmlNewChild (URL, NULL, BAD_CAST "message",  BAD_CAST  result_arg.group[num].message );

            xmlAddChild (group, URL);
        }

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* C.2.27 云镜控制- ——解析 ********************************/
    char* ControlPTZ(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node;
        xmlChar *value;
        char *command;
        char *content;
        struct ControlPTZ req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }
        /******* 修改下面的内容提取参数到 struct ControlPTZ req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"resId")==0)          //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userId")==0)          //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userLevel")==0)          //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userLevel, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"cmd")==0)          //get cmd node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.cmd, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"param")==0)          //get param node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.param, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"speed")==0)    //get speed node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
  				req_args.speed = atoi(content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct ControlPTZ req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.ControlPTZ(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "resId",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* C.2.28 跨节点资源下发- ——解析 ********************************/
    char* ResTransOrder(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl;
        xmlChar *value;
        char *command;
        char *content;
        struct ResTransOrder req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }
        /******* 修改下面的内容提取参数到 struct ResTransOrder req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0)          //get muId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"totalPacketNum")==0)          //get totalPacketNum node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.totalPacketNum, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"curPacketNum")==0)          //get curPacketNum node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.curPacketNum, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"resources") == 0)     //get resources node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"resId")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.resId, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"name")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.name, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"location")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.location, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"purpose")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.purpose, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"information")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.information, content);
                        xmlFree(value);
                    }
                }//end traverse gurl node
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct ResTransOrder req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.ResTransOrder(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* C.2.29 跨节点资源变更- ——解析 ********************************/
    char* ResChangeOrder(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl;
        xmlChar *value;
        char *command;
        char *content;
        struct ResChangeOrder req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }
        /******* 修改下面的内容提取参数到 struct ResTransOrder req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0)          //get muId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"cmd")==0)          //get cmd node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.cmd, content);
                xmlFree(value);
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST"resources") == 0)     //get resources node
            {
                for(gurl=node->children; gurl; gurl=gurl->next)   //traverse gurl node
                {
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"resId")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.resId, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"name")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.name, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"location")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.location, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"purpose")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.purpose, content);
                        xmlFree(value);
                    }
                    else if(xmlStrcasecmp(gurl->name,BAD_CAST"information")==0)
                    {
                        value = xmlNodeGetContent(node);
                        content = (char *)value;
                        strcpy(req_args.resources.information, content);
                        xmlFree(value);
                    }
                }//end traverse gurl node
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct ResChangeOrder req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.ResChangeOrder(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* //C.2.8 实时流媒体传输 核心DDU->区域DDu ——解析 ********************************/
    char* INFO(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node;
        xmlChar *value;
        char *command;
        char *content;
        struct INFO req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }
        /******* 修改下面的内容提取参数到 struct INFO req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"sessionId")==0)          //get sessionId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.sessionId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"resId")==0)          //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userId")==0)          //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userLevel")==0)          //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userLevel, content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct INFO req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.INFO(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* //C.2.11 历史视频流媒体传输 核心DDU->区域DDu ——解析 ********************************/
    char* HisInfo(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node;
        xmlChar *value;
        char *command;
        char *content;
        struct INFO req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }
        /******* 修改下面的内容提取参数到 struct INFO req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"sessionId")==0)          //get sessionId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.sessionId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"resId")==0)          //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userId")==0)          //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userLevel")==0)          //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userLevel, content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct INFO req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.HisInfo(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

/************************* //C.2.14 历史视频下载流媒体传输 核心DDU->区域DDu ——解析 ********************************/
    char* HisLoadInfo(char *xmlbody, int len)
    {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node;
        xmlChar *value;
        char *command;
        char *content;
        struct INFO req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL)
        {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next)
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL)
        {
            printf("no node = parameters\n");
            exit(1);
        }
        /******* 修改下面的内容提取参数到 struct INFO req_args  *************/
        for(node=node->children; node; node=node->next)   //有可能顺序不确定
        {
            if(xmlStrcasecmp(node->name,BAD_CAST"sessionId")==0)          //get sessionId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.sessionId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"resId")==0)          //get resId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.resId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userId")==0)          //get userId node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userId, content);
                xmlFree(value);
            }
            else if(xmlStrcasecmp(node->name,BAD_CAST"userLevel")==0)          //get userLevel node
            {
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.userLevel, content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
 /*******step 2 --------通过 回调函数获得 返回值， 参数 （ struct INFO req_args) *********/
        struct result result_arg;                         /*********  返回 值参数 ～～～～～～～～～～～～～*/
        result_arg = pcb_FunMap.HisLoadInfo(req_args);    /*** 调用回调函数构造 返回值 ～～～～～～～～～～*/

/****** step 3-  构造响应 xml_body ********/
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
         xmlNodePtr parameters_node;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);
        //先创建后添加
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameters");
        xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId",  BAD_CAST result_arg.args );
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

          /******************* 修改完成 *********************************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }


};

typedef char* (XMLMessageParse::*CBMemberFunction)(char *, int);//根据不同 command 解析 xml 的 Callback函数原型

class FuncCode//函数的从属关系
{
public:
    FuncCode(XMLMessageParse* pObj, CBMemberFunction pFun)
    {
        obj=pObj;
        fun=pFun;
    }
public:
    XMLMessageParse* obj;
    CBMemberFunction fun;
};

class DealMessage//调用类
{
public:
    DealMessage()
    {}
    ~DealMessage()
    {
        FunMapIterator itend=funmap.end();
        for (FunMapIterator it=funmap.begin (); it!=itend; it++)
        {
            delete it->second;
        }
        funmap.clear ();
    }

    int addMessageFunction(std::string msg, char * (XMLMessageParse::*fun)(char *, int), XMLMessageParse* pobj)
    {
        funmap[msg]=new FuncCode(pobj, fun);
        return 1;
    }
    char* operator()(std::string msg, char* para1,int para2)
    {
        return ((funmap[msg]->obj)->*(funmap[msg]->fun))(para1,para2);
    }
protected:
    typedef std::map<std::string,FuncCode*> FunMap;
    typedef std::map<std::string,FuncCode*>::iterator FunMapIterator;
    FunMap funmap;
};

#endif //XML_PARSE_H
