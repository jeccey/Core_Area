#include "../include/My_connec.h"

My_connec::My_connec(evutil_socket_t fd, struct bufferevent * bev, void* server)
    :Connection(fd, bev, server)
{
    msghdler = new Message_handler();
    XMLMessageParse* pObj= new XMLMessageParse();

    {
        //初始化， 注册不同 command 参数所对应的解析xml的回调函数
        dealMsg.addMessageFunction("MURegister", &XMLMessageParse::MURegister, pObj);
        dealMsg.addMessageFunction ("ResReport", &XMLMessageParse::ResReport, pObj);
        dealMsg.addMessageFunction ("ResChange", &XMLMessageParse::ResChange, pObj);
        dealMsg.addMessageFunction("MUKeepAlive", &XMLMessageParse::MUKeepAlive, pObj);
        dealMsg.addMessageFunction("ReportCamResState", &XMLMessageParse::ReportCamResState, pObj);
        dealMsg.addMessageFunction("UserResReport", &XMLMessageParse::UserResReport, pObj);
        dealMsg.addMessageFunction("UserResChange", &XMLMessageParse::UserResChange, pObj);
        dealMsg.addMessageFunction("AlarmResListReport", &XMLMessageParse::AlarmResListReport, pObj);
        dealMsg.addMessageFunction("AlarmResListChange", &XMLMessageParse::AlarmResListChange, pObj);

        dealMsg.addMessageFunction("ReportAlarmRes", &XMLMessageParse::ReportAlarmRes, pObj);

    }
};

My_connec::~My_connec()
{}

//服务器接收到数据 data, 长度为 numBytes
void My_connec::onRead(const char* data, const size_t& numBytes)
{
    string sx_msg;
    const char *d = "||";
    char *p;
    p = strtok((char *)data,d);
    while(p) {
        sx_msg = p;
        if(sx_msg.find("INVITE") == string::npos) { //响应消息，忽略即可, 暂时先打印以供调试
            std::cout<<"------- reply message from client:--------"<<std::endl<<sx_msg;
        } else {//请求消息
            std::cout<<"----------------------client request msg:--------------"<<std::endl<<sx_msg;
            string send_sx_msg;
            char *command;
            char *xml_body;
//1.------------------生成部分响应 的 sip头部----------------------------------------------------------------------------
            //调用str_to_msg, 提取字符串sx_msg中的部分信息填充到 响应的sip_msg 结构体中（剩余部分在发送之前填完）
            (*msghdler).str_to_sipmsg(sip_msg,  sx_msg);

//2.----------------------解析 xml 信息，并生成响应的xml_body--------------------------------------------
            //解析xml_body, 提取 command 参数, 根据不同command ,调用不同的回调函数进行最终解析，
            xmlDocPtr doc = xmlParseMemory(sip_msg.xml_body, sip_msg.content_length);

            xmlNodePtr request = xmlDocGetRootElement(doc);                                         //根据xmldoc获得xml的根节点
            command = (char *)xmlGetProp(request, (const xmlChar *)"command");  //提取command属性的值

            //根据不同command ,调用不同的回调函数, 返回响应的  char *xml_body
            xml_body = dealMsg(command, sip_msg.xml_body, sip_msg.content_length);

            xmlFreeDoc(doc);
            xmlCleanupParser();
            xmlMemoryDump();

            if(xml_body == NULL) {
                //错误处理，字符串格式不对，考虑 如何处理
            }

//3. ------------------------构造响应 字符串：send_sx_msg-----------------------------------------------
            //填充未完成的sip头部
            (*msghdler).create_reply_header(sip_msg, "200", "OK", strlen(xml_body));
            sip_msg.xml_body = xml_body;

            //调用sipmsg_to_str, 将sip_msg信息转换为一个字符串，保留在字符串 send_sx_msg中
            (*msghdler).msg_to_str(send_sx_msg, &sip_msg);
//4.-----------------------发送回对应的客户端----------------------------------------------------------------
            send(send_sx_msg.c_str(), send_sx_msg.length());
        }

        p=strtok(NULL,d);

    }
}

