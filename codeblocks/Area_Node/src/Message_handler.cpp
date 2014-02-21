    #include "../include/Message_handler.h"
#include <iostream>
#include <sstream>

Message_handler::Message_handler()
{
    //ctor
}

Message_handler::~Message_handler()
{
    //dtor
}

int  Message_handler::msg_to_str(string &sx_msg, struct sip_message *sipmsg)
{
    sx_msg.clear();
    sx_msg += "||";
    std::ostringstream oss;
    if ((*sipmsg).first_line.type == 0){ //request sip msg
        sx_msg += (*sipmsg).first_line.u.request.method;
        sx_msg += " sip:";
        sx_msg += (*sipmsg).first_line.u.request.uri;
        sx_msg += " SIP/2.0\r\nVia: SIP/2.0/TCP ";
        sx_msg += (*sipmsg).via;
        sx_msg += "\r\nTo: ";
        sx_msg += (*sipmsg).to;
        sx_msg += "\r\nFrom: ";
        sx_msg += (*sipmsg).from;
        sx_msg += "\r\nMax-Forwards: 70\r\nCall-ID: ";
        sx_msg += (*sipmsg).call_info;
        sx_msg += "\r\nCSeq: ";
        oss <<(*sipmsg).cseq;
        sx_msg += oss.str();
        oss.clear();
        oss.str("");
        sx_msg.append(" ");
        sx_msg += (*sipmsg).first_line.u.request.method;
        sx_msg += "\r\nContent-Type: RVSS/xml\r\n";
        sx_msg += "Content-Length: ";
        oss <<(*sipmsg).content_length;
        sx_msg += oss.str();
        sx_msg += "\r\n\r\n";
        sx_msg += (*sipmsg).xml_body;
    } else {//response sip msg
        sx_msg += "SIP/2.0 ";
        sx_msg += (*sipmsg).first_line.u.reply.status;
        sx_msg.append(" ");
        sx_msg += (*sipmsg).first_line.u.reply.reason;

        sx_msg += "\r\nVia: SIP/2.0/TCP ";
        sx_msg += (*sipmsg).via;
        sx_msg += "To: "; // sx_msg += "\r\nTo: ";
        sx_msg += (*sipmsg).to;
        sx_msg += "From: ";// sx_msg += "\r\nFrom: ";
        sx_msg += (*sipmsg).from;
        sx_msg += "Call-ID: " ;//sx_msg += "\r\nCall-ID: " ;
        sx_msg += (*sipmsg).call_info ;

        sx_msg += "CSeq: ";//sx_msg += "\r\nCSeq: ";
        oss <<(*sipmsg).cseq;
        sx_msg += oss.str();
        oss.clear();
        oss.str("");

        sx_msg.append(" ");
        sx_msg += (*sipmsg).first_line.u.request.method;
        sx_msg += "\r\nContent-Type: RVSS/xml\r\nContent-Length: ";
        oss <<(*sipmsg).content_length;
        sx_msg += oss.str();
        sx_msg += "\r\n\r\n";
        sx_msg += (*sipmsg).xml_body;

    }

    return 0;
}

//将字符串sx_msg 转换为sip_msg信息
void Message_handler::str_to_sipmsg(struct sip_message &sipmsg, const string &str_sxmsg)
{
   int pos = 0;
    int len = 0;

    //提取 via
    char *cvia = new char[50];
    memset(cvia, '\0', 50);
    pos = str_sxmsg.find("Via");
    len = str_sxmsg.find("To") - pos;
    str_sxmsg.copy(cvia, len, pos);
    sipmsg.via = cvia;

    //提取 from 写成 to
    char *cto = new char[60];
    memset(cto, '\0', 60);
    pos = str_sxmsg.find("From") + 6;
    len = str_sxmsg.find("Max") - pos;
    str_sxmsg.copy(cto, len, pos);
    sipmsg.to = cto;

    //提取 to 写成 from
    char *cfrom = new char[60];
    memset(cfrom, '\0', 60);
    pos = str_sxmsg.find("To") + 4;
    len = str_sxmsg.find("From") - pos;
    str_sxmsg.copy(cfrom, len, pos);
    sipmsg.from = cfrom;

    //提取 Call-ID
    char *ccall = new char[80];
    memset(ccall, '\0', 80);
    pos = str_sxmsg.find("Call") + 9;
    len = str_sxmsg.find("CSeq") - pos;
    str_sxmsg.copy(ccall, len, pos);
    sipmsg.call_info = ccall;

    //提取 CSeq, 注意是 int 类型的
    char *ccseq = new char[10];
    pos = str_sxmsg.find("CSeq");
    len = str_sxmsg.find(" INVITE") - pos-6;
    str_sxmsg.copy(ccseq, len, pos+6);
    sipmsg.cseq = atoi(ccseq) + 1;

    //设置 content_type
    sipmsg.content_type = "RVSS/xml";

    //提取xml_body
    char *cxml = new char[str_sxmsg.length()];
    memset(cxml, '\0', str_sxmsg.length());
    pos = str_sxmsg.find("\r\n\r\n");

    len = str_sxmsg.length()-pos-4;
    str_sxmsg.copy(cxml, len, pos+4);
    sipmsg.xml_body = cxml;

    //接收到的xml_body 长度
    sipmsg.content_length = len;
}


void Message_handler::create_request_header(struct sip_message &sip_msg, char *uri
        , char *via, char *to, char* from, char *call_id)
{
    sip_msg.first_line.type = 0;    //type == 0, 表示request
    sip_msg.first_line.u.request.method = "INVITE";
    sip_msg.first_line.u.request.version = "SIP/2.0";
    sip_msg.first_line.u.request.uri = uri;

    sip_msg.via = via;
    sip_msg.to = to;
    sip_msg.from = from;
    sip_msg.maxforwards = "70";
    sip_msg.call_info = call_id;
    sip_msg.content_type = "RVSS/xml";

    //构建xml消息体时设置 cseq, Content-Length
}

void Message_handler::create_reply_header(struct sip_message &sip_msg, char *status, char *reason, int content_length)
{
    sip_msg.first_line.type = 1;
    sip_msg.first_line.u.reply.version = "SIP/2.0";
    sip_msg.first_line.u.reply.status = status;
    sip_msg.first_line.u.reply.reason = reason;

    sip_msg.content_length = content_length;

    //构建xml消息体时设置 cseq, Content-Length
}


xmlDocPtr Message_handler::create_XML_request(const char *command)
{
    xmlDocPtr doc = NULL;           	/* document pointer */
    xmlNodePtr root_node = NULL;     /* node pointer*/

    //1. Creates a new document, a node and set it as a root node
    doc = xmlNewDoc(BAD_CAST "1.0");
    doc->encoding = BAD_CAST strdup("GB2312");
    doc->standalone = 1;	//standalone = yes

    root_node = xmlNewNode(NULL, BAD_CAST "request");// request or response
    xmlDocSetRootElement(doc, root_node);   /* set document's root node */

    xmlNewProp(root_node, BAD_CAST "command", BAD_CAST command);
    xmlNewChild (root_node, NULL, BAD_CAST "parameters", NULL);
    return doc;
}

