#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#include "Parameters.h"

using namespace std;

struct msg_start {
    int type;                 /* Type of the Message: 0-Request, 1-Response */
    //int len;                  /* length including delimiter */
    union {
        struct { // 请求行（方法 Request-URI 版本）
            char* method;       /* Method string */
            char* uri;          /* Request URI */
            char* version;      /* SIP version */
            int method_value;
        } request;

        struct { //响应行（版本 状态码 信息）
            char* version;      /* SIP version */
            char* status;       /* Reply status */
            char* reason;       /* Reply reason phrase */
            unsigned int statuscode;    /* statusclass,*/
        } reply;
    } u;
};

struct sip_message {
    struct msg_start first_line;   /* Message first line,开始行(请求行、状态行)	 */
    //message-header
    char* via;
    char* to;
    char* from;
    char* maxforwards;
    char* call_info;
    char* content_type;
    int cseq;
    int content_length;
    //CRLF
    //message-body
    char *xml_body;
    //unsigned int len; /* message len */
};

class Message_handler
{
public:
    Message_handler();
    virtual ~Message_handler();

    void create_request_header(struct sip_message &sip_msg, char *uri
                               , char *via, char *to, char* from, char *call_id);

    void create_reply_header(struct sip_message &sip_msg, char *status, char *reason, int content_length);

    int msg_to_str(string &sx_msg, struct sip_message *sipmsg);
    void str_to_sipmsg(struct sip_message &sipmsg, const string &sx_msg);

    xmlDocPtr create_XML_request(const char *command);
    //xmlDocPtr create_XML_repley(const char *command);

private:
//    struct CAInformation ca;

};

#endif // MESSAGE_HANDLER_H


