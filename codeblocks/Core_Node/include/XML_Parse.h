#ifndef XML_PARSE_H
#define XML_PARSE_H

#include <string>
#include <iostream>
#include <map>


#include <libxml/parser.h>
#include <libxml/tree.h>
#include "Parameters.h"

#include "Core_callback.h"

/*功能类, 提供回调函数实现，调用对象（根据不同 command 解析 xml 的 Callback函数）*/
class XMLMessageParse
{
public:
    XMLMessageParse()
    {}
    ~XMLMessageParse()
    {}
    xmlDocPtr create_XML_reply(const char *command) {

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

    /************************* 2.5 SIP注册——解析 ***********************************/
    char* MURegister(char *xmlbody, int len) {
        xmlDocPtr doc;
        xmlNodePtr request, node;
        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令
        struct MUInfo mu_info;	/**** xml解析结果，作为回调函数的参数以得到返回值 ***/

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }
        /************** 修改下面的内容提取参数 到struct MUInfo mu_info******************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name, BAD_CAST"muIp")==0) { //get muIp node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.muIp, content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"muPort")==0) { //get muPort node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.muPort, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name, BAD_CAST"muName")==0) { //get muName node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.muName, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name, BAD_CAST"muPassword")==0) { //get muPassword node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.muPassword, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"manufacturerld")==0) { //get manufacturerld node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.manufacturerld, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"manufacturerName")==0) { //get manufacturerName node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.manufacturerName, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"productVersion")==0) { //get productVersion node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.productVersion, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"softwareVersion")==0) { //get softwareVersion node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(mu_info.softwareVersion, content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2----------------- 通过 回调函数获得 返回值 ----------------------------------------------------------------------------------
        struct result result_arg;                                                /*** 返回 值 *****************/
        result_arg = pcb_FunMap.MURgister(mu_info);    /*** 调用回调函数构造 返回值 ***/

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
        // 添加子节点 parameters_node 到 result_root 节点
        xmlAddChild (result_root, parameters_node);

        /***************  修改完毕    ************************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } // MuRegister

    /************************* 2.2 目录资源上报——解析 ********************************/
    char* ResReport(char *xmlbody, int len) {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct ResReportArgs ResReportArgs;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct ResReportArgs ResReportArgs  *************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(ResReportArgs.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"totalPacketNum")==0) { //get totalPacketNum node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                ResReportArgs.totalPacketNum = atoi(content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"curPacketNum")==0) { //get curPacketNum node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                ResReportArgs.curPacketNum = atoi(content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0) { //get curPacketNum node
                for(gurl=node->children; gurl; gurl=gurl->next) { //traverse gurl node
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0) {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"resId")==0) { //get resId node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(ResReportArgs.group[i].resId, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"name")==0) {        //get name node
                                value=xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(ResReportArgs.group[i].name, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"location")==0) {        //get location node
                                value=xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(ResReportArgs.group[i].location, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"purpose")==0) {        //get purpose node
                                value=xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(ResReportArgs.group[i].purpose, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"information")==0) {        //get information node
                                value=xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(ResReportArgs.group[i].information, content);
                                xmlFree(value);
                            }
                        i++;
                    }
                }//end traverse gurl node
            }
        }
        //printf("------------ResReport callback-------------------\n");
        //printf("%s\n%d\n%d\n", ResReportArgs.muId, ResReportArgs.totalPacketNum, ResReportArgs.curPacketNum);
        //printf("%s\n%s\n%s\n%s\n%s\n", ResReportArgs.group[0].resId, ResReportArgs.group[0].name, ResReportArgs.group[0].location, ResReportArgs.group[0].purpose, ResReportArgs.group[0].information);

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2 --------通过 回调函数获得 返回值， 参数 （ struct ResReportArgs ResReportArgs）
        struct result result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.ResReport(ResReportArgs);    /*** 调用回调函数构造 返回值 ***/

//step 3--------------------------------------------------------------------------------------------------
        // 构造响应 xml_body
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);
        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        /*************** 修改完成 ******************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } //ResReport 2.2 目录资源上报——解析

    /************************* 2.3 目录资源变更 —— 解析 ********************************/
    char* ResChange(char *xmlbody, int len) {
        struct ResChangeArgs res_change_args;
        xmlDocPtr doc;
        xmlNodePtr request, node;
        xmlNodePtr resources;					//~~
        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }
//-----------------------------修改下面的----------------------------------------------------------------------------------------
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(res_change_args.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"cmd")==0) { //get cmd node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(res_change_args.cmd, content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"resources") == 0) { //get curPacketNum node
                for(resources=node->children; resources; resources=resources->next) { //traverse resources node
                    if(xmlStrcasecmp(resources->name,BAD_CAST"resId")==0) { //get resId node
                        value = xmlNodeGetContent(resources);
                        content = (char *)value;
                        strcpy(res_change_args.resources.resId, content);
                        xmlFree(value);
                    } else if(xmlStrcasecmp(resources->name,BAD_CAST"name")==0) { //get name node
                        value=xmlNodeGetContent(resources);
                        content = (char *)value;
                        strcpy(res_change_args.resources.name, content);
                        xmlFree(value);
                    } else if(xmlStrcasecmp(resources->name,BAD_CAST"location")==0) { //get location node
                        value=xmlNodeGetContent(resources);
                        content = (char *)value;
                        strcpy(res_change_args.resources.location, content);
                        xmlFree(value);
                    } else if(xmlStrcasecmp(resources->name,BAD_CAST"purpose")==0) { //get purpose node
                        value=xmlNodeGetContent(resources);
                        content = (char *)value;
                        strcpy(res_change_args.resources.purpose, content);
                        xmlFree(value);
                    } else if(xmlStrcasecmp(resources->name,BAD_CAST"information")==0) { //get information node
                        value=xmlNodeGetContent(resources);
                        content = (char *)value;
                        strcpy(res_change_args.resources.information, content);
                        xmlFree(value);
                    }
                }//end traverse resources node
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2--------通过 回调函数获得 返回值
        struct result result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.ResChange(res_change_args);    /*** 调用回调函数构造 返回值 ***/


//step 3--------------------------------------------------------------------------------------------------
        // 构造响应 填充好部分转换为sip_msg信息结构体
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);

        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } // ResChange

    /************************* C2.6 心跳 —— 解析 ********************************/
    char* MUKeepAlive(char *xmlbody, int len) {
        struct MUKeepAlive muKeepAlive_args;	//请求参数,提取到该结构体中
        xmlDocPtr doc;
        xmlNodePtr request, node;				//~~
        xmlChar *value;	//节点内容
        char *content;	//节点内容
        char *command;	//对应的 command 命令

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }
//------------    修改下面的       -----------------------------------------------
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muKeepAlivePeriod")==0) {        //get muKeepAlivePeriod node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(muKeepAlive_args.muKeepAlivePeriod, content);
                xmlFree(value);
            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
//step 2--------通过 回调函数获得 返回值
        struct result result_arg;              /*** 返回 值 *****************/
        result_arg = pcb_FunMap.MUKeepAlive(muKeepAlive_args);    /*** 调用回调函数构造 返回值 ***/


//step 3--------------------------------------------------------------------------------------------------
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
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        //添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muKeepAlivePeriod",  BAD_CAST result_arg.value );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);


        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);
        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    } // MUKeepAlive

    /************************* C.2.15 摄像机资源上报——解析 ********************************/
    char* ReportCamResState(char *xmlbody, int len) {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct ReportCamResState req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct ResReportArgs req_args  *************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0) { //get group node
                for(gurl=node->children; gurl; gurl=gurl->next) { //traverse gurl node
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0) {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"resId")==0) { //get resId node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].resId, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"state")==0) { //get state node
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
        struct result_ReportCamResState result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.ReportCamResState(req_args);    /*** 调用回调函数构造 返回值 ***/

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
        for(num = 0; num<result_arg.urlNum; num++) {
            URL = NULL;
            URL = xmlNewNode (NULL, BAD_CAST "URL");
            xmlNewChild (URL, NULL, BAD_CAST "resId",  BAD_CAST  result_arg.group[num].resId );
            //xmlNewChild (URL, NULL, BAD_CAST "state",  BAD_CAST  result_arg.group[num].state );
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

    /************************* 2.17 用户资源上报——解析 ********************************/
    char* UserResReport(char *xmlbody, int len) {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct UserResReport req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct UserResReport req_args  *************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"muName")==0) {      //get muName node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muName, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"totalPkt")==0) { //get totalPkt node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.totalPkt = atoi(content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"pktNum")==0) { //get pktNum node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.pktNum = atoi(content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0) { //get group node
                for(gurl=node->children; gurl; gurl=gurl->next) { //traverse gurl node
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0) {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0) { //get id node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"name")==0) { //get name node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].name, content);
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
        struct result result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.UserResReport(req_args);    /*** 调用回调函数构造 返回值 ***/

//step 3--------------------------------------------------------------------------------------------------
        // 构造响应 xml_body
        xmlChar *xml_body;
        int xml_body_len;
        xmlDocPtr rdoc;
        xmlNodePtr result_root;
        char int_to_str[10];

        rdoc = create_XML_reply(command);
        result_root =  xmlDocGetRootElement(rdoc);
        /*************** 修改下面的以构造响应 xml ******************/
        //创建新的子节点
        xmlNodePtr result_node;
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        /*************** 修改完成 ******************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

    /************************* 2.19 用户资源变更——解析 ********************************/
    char* UserResChange(char *xmlbody, int len) {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct UserResChange req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct UserResChange req_args  *************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"totalPkt")==0) { //get totalPkt node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.totalPkt = atoi(content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"pktNum")==0) { //get pktNum node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.pktNum = atoi(content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"cmd")==0) { //get cmd node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.cmd = atoi(content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0) { //get group node
                for(gurl=node->children; gurl; gurl=gurl->next) { //traverse gurl node
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0) {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0) { //get id node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"name")==0) { //get name node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].name, content);
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
        struct result result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.UserResChange(req_args);    /*** 调用回调函数构造 返回值 ***/

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
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        //添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.args );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);
        /*************** 修改完成 ******************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }


    /************************* 2.21 告警资源上报——解析 ********************************/
    char* AlarmResListReport(char *xmlbody, int len) {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct AlarmResList req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct AlarmResList req_args  *************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"muName")==0) {      //get muName node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muName, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"totalPkt")==0) { //get totalPkt node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.totalPkt = atoi(content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"pktNum")==0) { //get pktNum node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.pktNum = atoi(content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0) { //get group node
                for(gurl=node->children; gurl; gurl=gurl->next) { //traverse gurl node
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0) {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0) { //get id node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"name")==0) { //get name node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].name, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"description")==0) { //get description node
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
//step 2 --------通过 回调函数获得 返回值， 参数 （ struct ResReportArgs req_args
        struct result result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.AlarmResListReport(req_args);    /*** 调用回调函数构造 返回值 ***/

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
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        //添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.args );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);


        /*************** 修改完成 ******************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }


    /************************* 2.22 告警资源变更——解析 ********************************/
    char* AlarmResListChange(char *xmlbody, int len) {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl;
        xmlChar *value;
        char *command;
        char *content;
        struct AlarmResListChange req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct AlarmResListChange req_args  *************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"muName")==0) {      //get muName node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muName, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"num")==0) {      //get num node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.num, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"totalPkt")==0) { //get totalPkt node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.totalPkt = atoi(content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"pktNum")==0) { //get pktNum node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.pktNum = atoi(content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"cmd")==0) { //get cmd node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                req_args.cmd = atoi(content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0) { //get group node
                for(gurl=node->children; gurl; gurl=gurl->next) { //traverse gurl node
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0) {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0) { //get id node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"name")==0) { //get name node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].name, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"type")==0) { //get type node
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
//step 2 --------通过 回调函数获得 返回值， 参数 （ struct ResReportArgs req_args
        struct result result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.AlarmResListChange(req_args);    /*** 调用回调函数构造 返回值 ***/

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
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        //添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.args );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);
        /*************** 修改完成 ******************/

        /* Dump the document to a buffer */
        xmlDocDumpFormatMemory(rdoc, &xml_body, &xml_body_len, 1);

        xmlFreeDoc(rdoc);
        xmlCleanupParser();     //清理
        xmlMemoryDump();        //debug memory for regression tests

        return (char *)xml_body;
    }

    /************************* C.2.24 告警信息上报 ——解析 ********************************/
    char* ReportAlarmRes(char *xmlbody, int len) {
        //解析xml_body, 提取参数
        xmlDocPtr doc;
        xmlNodePtr request,node,gurl, curl, ccurl;
        xmlChar *value;
        char *command;
        char *content;
        struct ReportAlarmRes req_args;     /**** xml解析结果，作为回调函数的参数以得到返回值 ***/
        int i = 0;

        xmlKeepBlanksDefault(0);
        doc = xmlParseMemory(xmlbody, len);////parse xml in memory
        if(doc==NULL) {
            printf("doc == null\n");
            exit(1);
        }
        //根据xmldoc获得xml的根节点 request
        request=xmlDocGetRootElement(doc);
        command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

        for(node=request->children; node; node=node->next) {
            if(xmlStrcasecmp(node->name,BAD_CAST"parameters")==0)
                break;
        }
        if(node==NULL) {
            printf("no node = parameters\n");
            exit(1);
        }

        /******* 修改下面的内容提取参数到 struct ReportAlarmRes req_args  *************/
        for(node=node->children; node; node=node->next) { //有可能顺序不确定
            if(xmlStrcasecmp(node->name,BAD_CAST"muId")==0) {        //get muId node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muId, content);
                xmlFree(value);
            } else if(xmlStrcasecmp(node->name,BAD_CAST"muName")==0) {      //get muName node
                value = xmlNodeGetContent(node);
                content = (char *)value;
                strcpy(req_args.muName, content);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST"group") == 0) { //get group node
                for(gurl=node->children; gurl; gurl=gurl->next) { //traverse gurl node
                    if(xmlStrcasecmp(gurl->name,BAD_CAST"URL")==0) {
                        for(curl = gurl->children; curl; curl=curl->next)
                            if(xmlStrcasecmp(curl->name,BAD_CAST"id")==0) { //get id node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].id, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"type")==0) { //get type node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].type, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"startTime")==0) { //get startTime node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].startTime, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"endTime")==0) { //get endTime node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].endTime, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"targetInfo")==0) { //get targetInfo node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].targetInfo, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"state")==0) { //get state node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].state, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"description")==0) { //get description node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                strcpy(req_args.group[i].description, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"level")==0) { //get level node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                req_args.group[i].level = atoi(content);
                                strcpy(req_args.group[i].description, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"alarmHisRecord")==0) { //get alarmHisRecord node
                                value = xmlNodeGetContent(curl);
                                content = (char *)value;
                                req_args.group[i].alarmHisRecord = atoi(content);
                                strcpy(req_args.group[i].description, content);
                                xmlFree(value);
                            } else if(xmlStrcasecmp(curl->name,BAD_CAST"url")==0) { //get url node
                                for(ccurl = curl->children; ccurl; ccurl=ccurl->next)
                                    if(xmlStrcasecmp(ccurl->name,BAD_CAST"resId")==0) { //get resId node
                                        value = xmlNodeGetContent(curl);
                                        content = (char *)value;
                                        strcpy(req_args.group[i].rtUrl.resId, content);
                                        xmlFree(value);
                                    } else if(xmlStrcasecmp(ccurl->name,BAD_CAST"time")==0) { //get time node
                                        value = xmlNodeGetContent(curl);
                                        content = (char *)value;
                                        strcpy(req_args.group[i].rtUrl.time, content);
                                        xmlFree(value);
                                    }
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
//step 2 --------通过 回调函数获得 返回值， 参数 （ struct ResReportArgs req_args
        struct result result_arg;                                                               /*** 返回 值 *****************/
        result_arg = pcb_FunMap.ReportAlarmRes(req_args);    /*** 调用回调函数构造 返回值 ***/

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
        result_node = xmlNewChild (result_root, NULL, BAD_CAST "result", BAD_CAST result_arg.value);
        sprintf(int_to_str,"%d",result_arg.code);
        xmlNewProp (result_node, (const xmlChar *)"code", (const xmlChar *)int_to_str);

        //添加子节点和子子节点
        parameters_node = xmlNewNode (NULL, BAD_CAST "parameter");
        xmlNewChild (parameters_node, NULL, BAD_CAST "muId",  BAD_CAST result_arg.args );
        //添加子节点 parameters_node 到 root 节点
        xmlAddChild (result_root, parameters_node);

        /*************** 修改完成 ******************/

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
    FuncCode(XMLMessageParse* pObj, CBMemberFunction pFun) {
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
    ~DealMessage() {
        FunMapIterator itend=funmap.end();
        for (FunMapIterator it=funmap.begin (); it!=itend; it++) {
            delete it->second;
        }
        funmap.clear ();
    }

    int addMessageFunction(std::string msg, char * (XMLMessageParse::*fun)(char *, int), XMLMessageParse* pobj) {
        funmap[msg]=new FuncCode(pobj, fun);
        return 1;
    }
    char* operator()(std::string msg, char* para1,int para2) {
        return ((funmap[msg]->obj)->*(funmap[msg]->fun))(para1,para2);
    }
protected:
    typedef std::map<std::string,FuncCode*> FunMap;
    typedef std::map<std::string,FuncCode*>::iterator FunMapIterator;
    FunMap funmap;
};

#endif //XML_PARSE_H
