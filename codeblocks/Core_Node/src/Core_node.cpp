#include "../include/Core_node.h"

struct pcbFunMap pcb_FunMap;

Core_node::Core_node(const char* cip, const unsigned short cport)
{
    ip = cip;
    port = cport;
    msghdler = new Message_handler();
}

Core_node::~Core_node()
{
    //dtor
}

void Core_node::run()
{

    server.setup(ip, port);
    server.start();
    server.update();
    printf("server run...\n");
}

void Core_node::create_request_header(char *uri, char *via, char *to, char* from, char *call_id)
{
    (*msghdler).create_request_header(sip_msg, uri, via, to, from, call_id);

}

/***************** C.2.4　历史视频查询 *****************************/
void Core_node::QueryHistoryFiles(const char *command, const struct reqHistoryFile *args,  char* ip)  /***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);   //根节点 <request command="MURegister">
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********/
    //创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cuId", BAD_CAST (*args).cuId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "fromDate", BAD_CAST (*args).fromDate);
    xmlNewChild (parameters_node, NULL, BAD_CAST "toDate", BAD_CAST (*args).toDate);
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/***************** C.2.7 请求实时媒体流 *****************************/
void Core_node::StartMediaReq(const char *command, const struct StartMediaReq *args, char* ip)  /***修改第二个参数***/
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
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).mediaType);
    xmlNewChild (parameters_node, NULL, BAD_CAST "mediaType",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).linkMode);
    xmlNewChild (parameters_node, NULL, BAD_CAST "linkMode",  (const xmlChar *) int_to_str);

    xmlNewChild (parameters_node, NULL, BAD_CAST "targetIpAddr", BAD_CAST (*args).targetIpAddr);
    xmlNewChild (parameters_node, NULL, BAD_CAST "targetPort", BAD_CAST (*args).targetPort);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).flag);
    xmlNewChild (parameters_node, NULL, BAD_CAST "flag",  (const xmlChar *) int_to_str);

    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/***************** C.2.9 停止实时媒体流传输 *****************************/
void Core_node:: StopMediaReq(const char *command, const struct StopMediaReq *args, char* ip)  /***修改第二个参数***/
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
    xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId", BAD_CAST (*args).sessionId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).stopFlag);
    xmlNewChild (parameters_node, NULL, BAD_CAST "stopFlag",  (const xmlChar *) int_to_str);

    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.10 请求历史视频回放 *****************************/
void Core_node::StartPlayBack(const char *command, const struct reqStartPlayBack *args, char* ip)  /***修改第二个参数***/
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
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);
    xmlNewChild (parameters_node, NULL, BAD_CAST "startTime", BAD_CAST (*args).startTime);
    xmlNewChild (parameters_node, NULL, BAD_CAST "endTime", BAD_CAST (*args).endTime);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).linkMode);
    xmlNewChild (parameters_node, NULL, BAD_CAST "linkMode",  (const xmlChar *) int_to_str);

    xmlNewChild (parameters_node, NULL, BAD_CAST "targetIpAddr", BAD_CAST (*args).targetIpAddr);
    xmlNewChild (parameters_node, NULL, BAD_CAST "targetPort", BAD_CAST (*args).targetPort);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).flag);
    xmlNewChild (parameters_node, NULL, BAD_CAST "flag",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).locationFlag);
    xmlNewChild (parameters_node, NULL, BAD_CAST "locationFlag",  (const xmlChar *) int_to_str);

    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.12 历史视频播放控制 *****************************/
void Core_node::ControlFileBack(const char *command, const struct req_ControlFileBack *args, char* ip) /***修改第二个参数***/
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
    xmlNewChild (parameters_node, NULL, BAD_CAST "sessioinId", BAD_CAST (*args).sessionId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cmd", BAD_CAST (*args).cmd);
    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).param);
    xmlNewChild (parameters_node, NULL, BAD_CAST "linkMode",  (const xmlChar *) int_to_str);

    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.13 请求历史视频下载 *****************************/
void Core_node::StartHisLoad(const char *command, const struct reqStartPlayBack *args, char* ip)  /***修改第二个参数***/
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
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);
    xmlNewChild (parameters_node, NULL, BAD_CAST "startTime", BAD_CAST (*args).startTime);
    xmlNewChild (parameters_node, NULL, BAD_CAST "endTime", BAD_CAST (*args).endTime);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).linkMode);
    xmlNewChild (parameters_node, NULL, BAD_CAST "linkMode",  (const xmlChar *) int_to_str);

    xmlNewChild (parameters_node, NULL, BAD_CAST "targetIpAddr", BAD_CAST (*args).targetIpAddr);
    xmlNewChild (parameters_node, NULL, BAD_CAST "targetPort", BAD_CAST (*args).targetPort);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).flag);
    xmlNewChild (parameters_node, NULL, BAD_CAST "flag",  (const xmlChar *) int_to_str);

    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).locationFlag);
    xmlNewChild (parameters_node, NULL, BAD_CAST "locationFlag",  (const xmlChar *) int_to_str);

    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}//StartHisLoad


/****************** C.2.16 摄像机状态查询 *****************************/
void Core_node::ReqCamResState(const char *command, const struct ReqCamResState *args, char* ip)/***修改第二个参数***/
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
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++) {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "resId",  BAD_CAST  (*args).group[num].resId );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** c.2.18 用户动态信息获取 *****************************/
void Core_node::GetUserCurState(const char *command, const struct UserCurState *args, char* ip)/***修改第二个参数***/
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
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "curUserId", BAD_CAST (*args).curUserId);

    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}


/******************c.2.20 摄像机资源屏蔽*****************************/
void Core_node::SetUserCamManage(const char *command, const struct SetUserCamManage *args, char* ip)/***修改第二个参数***/
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
    xmlNewChild (parameters_node, NULL, BAD_CAST "cuId", BAD_CAST (*args).cuId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cuLevel", BAD_CAST (*args).cuLevel);
    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).Action);
    xmlNewChild (parameters_node, NULL, BAD_CAST "Action",  (const xmlChar *) int_to_str);

    xmlNewChild (parameters_node, NULL, BAD_CAST "startTime", BAD_CAST (*args).startTime);
    xmlNewChild (parameters_node, NULL, BAD_CAST "endTime", BAD_CAST (*args).endTime);
    xmlNewChild (parameters_node, NULL, BAD_CAST "schduleCreatTime", BAD_CAST (*args).schduleCreatTime);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).camurl_Num; num++) {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "camId",  BAD_CAST  (*args).group[num].camId );
        xmlAddChild (group, URL);
    }

    //先创建，后添加方式
    xmlNodePtr whiteUser;
    whiteUser = xmlNewNode (NULL, BAD_CAST "whiteUser");
    xmlAddChild (parameters_node, whiteUser);
    for(num = 0; num<(*args).camurl_Num; num++) {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group_id[num].id );
        xmlAddChild (whiteUser, URL);
    }
    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}


/****************** C.2.23 告警资源订阅 *****************************/
void Core_node::AlarmResSubscribe(const char *command, const struct AlarmResSubscribe *args, char* ip)/***修改第二个参数***/
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
    sprintf(int_to_str,"%d",   (*args).action);
    xmlNewChild (parameters_node, NULL, BAD_CAST "action",  (const xmlChar *) int_to_str);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++) {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "type",  BAD_CAST  (*args).group[num].description );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.25 订阅告警的信息查询 *****************************/
void Core_node::QueryAlarmRes(const char *command, const struct QueryAlarmRes *args, char* ip)/***修改第二个参数***/
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
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muName", BAD_CAST (*args).muName);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++) {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "type",  BAD_CAST  (*args).group[num].type );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.26 告警信息查询 *****************************/
void Core_node::ReportAlarmInfo(const char *command, const struct ReportAlarmInfo *args, char* ip)/***修改第二个参数***/
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
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "muName", BAD_CAST (*args).muName);

    //先创建，后添加方式
    xmlNodePtr group;
    group = xmlNewNode (NULL, BAD_CAST "group");
    xmlAddChild (parameters_node, group);
    xmlNodePtr URL;
    int num;
    for(num = 0; num<(*args).urlNum; num++) {
        URL = NULL;
        URL = xmlNewNode (NULL, BAD_CAST "URL");
        xmlNewChild (URL, NULL, BAD_CAST "id",  BAD_CAST  (*args).group[num].id );
        xmlNewChild (URL, NULL, BAD_CAST "type",  BAD_CAST  (*args).group[num].type );
        xmlNewChild (URL, NULL, BAD_CAST "startTime",  BAD_CAST  (*args).group[num].startTime );
        xmlNewChild (URL, NULL, BAD_CAST "endTime",  BAD_CAST  (*args).group[num].endTime );

        // 添加子节点 parameters_node 到 root 节点
        xmlAddChild (group, URL);

    }
    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.27 云镜控制 *****************************/
void Core_node::ControlPTZ(const char *command, const struct ControlPTZ *args, char* ip)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    char int_to_str[10];    //辅助int 转化 为 字符串
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cmd", BAD_CAST (*args).cmd);
    xmlNewChild (parameters_node, NULL, BAD_CAST "param", BAD_CAST (*args).param);

    // int 需要转化
    memset(int_to_str, 0, sizeof(int_to_str));
    sprintf(int_to_str,"%d",   (*args).speed);
    xmlNewChild (parameters_node, NULL, BAD_CAST "speed",  (const xmlChar *) int_to_str);

    /*************** 修改结束 ****************************************************************/

    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.28 跨节点资源下发 *****************************/
void Core_node::ResTransOrder(const char *command, const struct ResTransOrder *args, char* ip)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "totalPacketNum", BAD_CAST (*args).totalPacketNum);
    xmlNewChild (parameters_node, NULL, BAD_CAST "curPacketNum", BAD_CAST (*args).curPacketNum);

    //先创建，后添加方式
    xmlNodePtr resources;
    resources = xmlNewNode (NULL, BAD_CAST "resources");
    xmlNewChild (resources, NULL, BAD_CAST "resId",  BAD_CAST  (*args).resources.resId );
    xmlNewChild (resources, NULL, BAD_CAST "name",  BAD_CAST  (*args).resources.name );
    xmlNewChild (resources, NULL, BAD_CAST "location",  BAD_CAST  (*args).resources.location );
    xmlNewChild (resources, NULL, BAD_CAST "purpose",  BAD_CAST  (*args).resources.purpose );
    xmlNewChild (resources, NULL, BAD_CAST "information",  BAD_CAST  (*args).resources.information );
    xmlAddChild (parameters_node, resources);


    /*************** 修改结束 ****************************************************************/
    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** C.2.29 跨节点资源变更*****************************/
void Core_node::ResChangeOrder(const char *command, const struct ResChangeOrder *args, char* ip)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "muId", BAD_CAST (*args).muId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "cmd", BAD_CAST (*args).cmd);

    //先创建，后添加方式
    xmlNodePtr resources;
    resources = xmlNewNode (NULL, BAD_CAST "resources");
    xmlNewChild (resources, NULL, BAD_CAST "resId",  BAD_CAST  (*args).resources.resId );
    xmlNewChild (resources, NULL, BAD_CAST "name",  BAD_CAST  (*args).resources.name );
    xmlNewChild (resources, NULL, BAD_CAST "location",  BAD_CAST  (*args).resources.location );
    xmlNewChild (resources, NULL, BAD_CAST "purpose",  BAD_CAST  (*args).resources.purpose );
    xmlNewChild (resources, NULL, BAD_CAST "information",  BAD_CAST  (*args).resources.information );
    xmlAddChild (parameters_node, resources);

    /*************** 修改结束 ****************************************************************/
    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}


/****************** C.2.8 实时流媒体传输*****************************/
void Core_node::INFO(const char *command, const struct INFO *args, char* ip)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId", BAD_CAST (*args).sessionId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);

    /*************** 修改结束 ****************************************************************/
    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}


/****************** C.2.11 历史视频媒体传输 *****************************/
void Core_node::HisInfo(const char *command, const struct INFO *args, char* ip)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId", BAD_CAST (*args).sessionId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);

    /*************** 修改结束 ****************************************************************/
    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}

/****************** c.2.14 历史视频下载传输*****************************/
void Core_node::HisLoadInfo(const char *command, const struct INFO *args, char* ip)/***修改第二个参数***/
{
    string sx_msg;
    xmlDocPtr doc = NULL;            /* document pointer */
    xmlNodePtr parameters_node = NULL;
    xmlChar *xml_body;
    int xml_body_len;

    doc = (*msghdler).create_XML_request(command);
    parameters_node =  xmlDocGetRootElement(doc);
    parameters_node = parameters_node->children;    //<parameters>节点

    /**************  修改下面的内容：构造请求 xml ***********************************************/
    //直接创建新的子节点
    xmlNewChild (parameters_node, NULL, BAD_CAST "sessionId", BAD_CAST (*args).sessionId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "resId", BAD_CAST (*args).resId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userId", BAD_CAST (*args).userId);
    xmlNewChild (parameters_node, NULL, BAD_CAST "userLevel", BAD_CAST (*args).userLevel);

    /*************** 修改结束 ****************************************************************/
    //------ 将构造好的 请求xml 转化为字符串 xml_body, 并填充完未完成的 sip 头部
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
    cout<<"---------the sxmsg server sent:------------------- "<<endl<<sx_msg<<endl;
    cout<<"sxmsg length: "<<sx_msg.length()<<endl;

    // 根据ip 发送到指定的 MU;
    if(ip == NULL) {
        server.sendToAllClients(sx_msg.c_str(), sx_msg.length());
    } else {
        server.sendToClient(ip, sx_msg.c_str(), sx_msg.length());
    }
}




