#include "../include/Area_node.h"

struct pcbFunMap pcb_FunMap;        //回调函数对应表

Area_node::Area_node(const char *cip, const unsigned short cport)
{
    cseq = 0;
    ip = cip;
    port = cport;
}

Area_node::~Area_node()
{
    //dtor
}


void Area_node::run()
{
	//connect to host
	tcli.conn(ip , port);

    while(1)
    {
	//cout<<"-------tcli receive msg---------------------\n";
        //cout<<tcli.receive(10240);
        tcli.receive(10240);
    }

}


void Area_node::create_request_header(char *uri, char *via, char *to, char* from, char *call_id)
{
    (*msghdler).create_request_header(sip_msg, uri, via, to, from, call_id);
}

/****************** C.2.5　SIP注册  区域节点向核心节点注册 *****************************/
void Area_node::MURegister(const char *command, const struct MUInfo *ca)     /***修改第二个参数***/
{
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;


    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********/
    //创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*ca).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muIp", BAD_CAST (*ca).muIp);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muPort", BAD_CAST (*ca).muPort);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muName", BAD_CAST (*ca).muName);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muPassword", BAD_CAST (*ca).muPassword);
    xmlNewChild (parameters_node, NULL, BAD_CAST "manufacturerId", BAD_CAST (*ca).manufacturerld);
    xmlNewChild (parameters_node, NULL, BAD_CAST "manufacturerName", BAD_CAST (*ca).manufacturerName);
    xmlNewChild (parameters_node, NULL, BAD_CAST "productVersion", BAD_CAST (*ca).productVersion);
    xmlNewChild (parameters_node, NULL, BAD_CAST "softwareVersion", BAD_CAST (*ca).softwareVersion);
    /*************** 修改结束 ****************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
}

/****************** C.2.2　区域节点向核心节点目录资源上报 *****************************/
void Area_node::ResReportArgs(const char *command, const struct ResReportArgs *resRpArgs)      /***修改第二个参数***/
{
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);       //根节点 <request command="ResReportArgs">
    parameters_node = parameters_node->children;     //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********/
    char int_to_str[10];    //辅助int 转化 为 字符串
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*resRpArgs).muId);
    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*resRpArgs).totalPacketNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "totalPacketNum",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*resRpArgs).curPacketNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "curPacketNum",  (const xmlChar *) int_to_str);

    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    //先创建，后添加方式
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*resRpArgs).urlNum; num++)
    {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "resId",  BAD_CAST  (*resRpArgs).group[num].resId );
        xmlNewChild (URL, NULL, BAD_CAST "name",  BAD_CAST  (*resRpArgs).group[num].name );
        xmlNewChild (URL, NULL, BAD_CAST "location",  BAD_CAST  (*resRpArgs).group[num].location );
        xmlNewChild (URL, NULL, BAD_CAST "purpose",  BAD_CAST  (*resRpArgs).group[num].purpose );
        xmlNewChild (URL, NULL, BAD_CAST "information",  BAD_CAST  (*resRpArgs).group[num].information );
        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
}

/****************** C.2.3　目录资源变更  *****************************/
void Area_node::ResChange(const char *command, const struct ResChangeArgs *resChangeArgs)     /***修改第二个参数***/
{
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********/
    //创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*resChangeArgs).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cmd", BAD_CAST (*resChangeArgs).cmd);
    //先创建，后添加方式
    xmlNodePtr resources;
    resources = xmlNewNode (NULL, BAD_CAST "resources");
    xmlAddChild (parameters_node, resources);

    xmlNewChild (resources, NULL, BAD_CAST "resId",  BAD_CAST  (*resChangeArgs).resources.resId );
    xmlNewChild (resources, NULL, BAD_CAST "name",  BAD_CAST  (*resChangeArgs).resources.name );
    xmlNewChild (resources, NULL, BAD_CAST "location",  BAD_CAST  (*resChangeArgs).resources.location );
    xmlNewChild (resources, NULL, BAD_CAST "purpose",  BAD_CAST  (*resChangeArgs).resources.purpose );
    xmlNewChild (resources, NULL, BAD_CAST "information",  BAD_CAST  (*resChangeArgs).resources.information );

    /*************** 修改结束 ****************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
   tcli.send_data(sx_msg.c_str(), sx_msg.length());
}

/****************** C.2.6　心跳  *****************************/
void Area_node::MUKeepAlive(const char *command, const struct MUKeepAlive *muKeepAlive)        /***修改第二个参数***/
{
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ******************************/
    //创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muKeepAlivePeriod", BAD_CAST (*muKeepAlive).muKeepAlivePeriod);

    /*************** 修改结束 ***********************************************/

    //---  将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部  --
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
   	tcli.send_data(sx_msg.c_str(), sx_msg.length());
}


/****************** C.2.15　摄像机状态上报 *****************************/
void Area_node::ReportCamResState(const char *command, const struct ReportCamResState *args)    /***修改第二个参数***/
{
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);       //根节点 <request command="ResReportArgs">
    parameters_node = parameters_node->children;     //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********/
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++)
    {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "resId",  BAD_CAST  (*args).group[num].resId );
        xmlNewChild (URL, NULL, BAD_CAST "state",  BAD_CAST  (*args).group[num].state );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
}

/****************** C.2.17 用户资源上报 *****************************/
void Area_node::UserResReport(const char *command, const struct UserResReport *args)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    char int_to_str[10];    //辅助int 转化 为 字符串
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muName", BAD_CAST (*args).muName);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).totalPkt);
    xmlNewChild (parameters_node, NULL, BAD_CAST "totalPkt",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).pktNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "pktNum",  (const xmlChar *) int_to_str);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++)
    {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "name",  BAD_CAST  (*args).group[num].name );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
 }

/****************** C.2.19 用户资源变更 *****************************/
void Area_node::UserResChange(const char *command, const struct UserResChange *args)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    char int_to_str[10];    //辅助int 转化 为 字符串
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).totalPkt);
    xmlNewChild (parameters_node, NULL, BAD_CAST "totalPkt",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).pktNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "pktNum",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).cmd);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cmd",  (const xmlChar *) int_to_str);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++)
    {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "name",  BAD_CAST  (*args).group[num].name );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
 }

/****************** c.2.21 告警资源上报 *****************************/
void Area_node::AlarmResListReport(const char *command, const struct AlarmResList *args)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    char int_to_str[10];    //辅助int 转化 为 字符串
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muName", BAD_CAST (*args).muName);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).totalPkt);
    xmlNewChild (parameters_node, NULL, BAD_CAST "totalPkt",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).pktNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "pktNum",  (const xmlChar *) int_to_str);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++)
    {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "name",  BAD_CAST  (*args).group[num].name );
        xmlNewChild (URL, NULL, BAD_CAST "description",  BAD_CAST  (*args).group[num].description );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
 }

/****************** c.2.22 告警资源变更 *****************************/
void Area_node::AlarmResListChange(const char *command, const struct AlarmResListChange *args)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    char int_to_str[10];    //辅助int 转化 为 字符串
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muName", BAD_CAST (*args).muName);
    xmlNewChild (parameters_node, NULL, BAD_CAST "num", BAD_CAST (*args).num);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).totalPkt);
    xmlNewChild (parameters_node, NULL, BAD_CAST "totalPkt",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).pktNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "pktNum",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).cmd);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cmd",  (const xmlChar *) int_to_str);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++)
    {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "name",  BAD_CAST  (*args).group[num].name );
        xmlNewChild (URL, NULL, BAD_CAST "type",  BAD_CAST  (*args).group[num].description );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
 }


/******************C.2.24 告警信息上报 *****************************/
void Area_node::ReportAlarmRes(const char *command, const struct ReportAlarmRes *args)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    char int_to_str[10];    //辅助int 转化 为 字符串
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muName", BAD_CAST (*args).muName);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++)
    {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "type",  BAD_CAST  (*args).group[num].type );
        xmlNewChild (URL, NULL, BAD_CAST "startTime",  BAD_CAST  (*args).group[num].startTime );
        xmlNewChild (URL, NULL, BAD_CAST "endTime",  BAD_CAST  (*args).group[num].endTime );
        xmlNewChild (URL, NULL, BAD_CAST "targetInfo",  BAD_CAST  (*args).group[num].targetInfo );

        // int 需要转化
        memset(int_to_str, 0, sizeof(int_to_str));
        sprintf(int_to_str,"%d",   (*args).group[num].level);
        xmlNewChild (URL, NULL, BAD_CAST "level",  (const xmlChar *) int_to_str);

        xmlNewChild (URL, NULL, BAD_CAST "state",  BAD_CAST  (*args).group[num].state );
        xmlNewChild (URL, NULL, BAD_CAST "description",  BAD_CAST  (*args).group[num].description );
        // int 需要转化
        memset(int_to_str, 0, sizeof(int_to_str));
        sprintf(int_to_str,"%d",   (*args).group[num].alarmHisRecord);
        xmlNewChild (URL, NULL, BAD_CAST "alarmHisRecord",  (const xmlChar *) int_to_str);

         //先创建，后添加方式
        xmlNodePtr url;
        url = xmlNewNode (NULL, BAD_CAST "url");
        xmlNewChild (url, NULL, BAD_CAST "resId",  BAD_CAST  (*args).group[num].rtUrl.resId );
        xmlNewChild (url, NULL, BAD_CAST "time",  BAD_CAST  (*args).group[num].rtUrl.time );
        xmlAddChild (URL, url);

        // 添加子节点 URL 到 group 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //----------------------------        将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
    /* Dump the document to a buffer */
    xmlDocDumpFormatMemory(doc, &xml_body, &xml_body_len, 1);

    sip_msg.content_length = xml_body_len;  //填充未完成的sip头部
    sip_msg.cseq = (++cseq) % 20;          //填充未完成的sip头部
    sip_msg.xml_body = (char *)xml_body;

    xmlFreeDoc(doc);
    xmlCleanupParser();     //清理
    xmlMemoryDump();        //debug memory for regression tests

    //调用msg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 sx_msg 中
    (*msghdler).msg_to_str(sx_msg, &sip_msg);

    /********************测试用************************************/
    cout<<"---------the sxmsg client sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 发送的核心节点服务器
    tcli.send_data(sx_msg.c_str(), sx_msg.length());
 }


