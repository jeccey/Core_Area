#include "../include/TCP_Client.h"

TCP_Client::TCP_Client()
{
    sock = -1;
    port = 0;
    address = "";
}

TCP_Client::~TCP_Client()
{
    //dtor
}

void TCP_Client::register_xmlparese()
{
    msghdler = new Message_handler();
    XMLMessageParse* pObj= new XMLMessageParse();

    {
        //初始化， 注册不同 command 参数所对应的解析xml的回调函数
        dealMsg.addMessageFunction("QueryHistoryFiles", &XMLMessageParse::QueryHistoryFiles, pObj);
        dealMsg.addMessageFunction("StartMediaReq", &XMLMessageParse::StartMediaReq, pObj);
        dealMsg.addMessageFunction("StopMediaReq", &XMLMessageParse::StopMediaReq, pObj);
        dealMsg.addMessageFunction("StartPlayBack", &XMLMessageParse::StartPlayBack, pObj);
        dealMsg.addMessageFunction("ControlFileBack", &XMLMessageParse::ControlFileBack, pObj);
        dealMsg.addMessageFunction("StartHisLoad", &XMLMessageParse::StartHisLoad, pObj);
        dealMsg.addMessageFunction("ReqCamResState", &XMLMessageParse::ReqCamResState, pObj);
        dealMsg.addMessageFunction("GetUserCurState", &XMLMessageParse::GetUserCurState, pObj);
        dealMsg.addMessageFunction("SetUserCamManage", &XMLMessageParse::SetUserCamManage, pObj);
        dealMsg.addMessageFunction("AlarmResSubscribe", &XMLMessageParse::AlarmResSubscribe, pObj);
        dealMsg.addMessageFunction("QueryAlarmRes", &XMLMessageParse::QueryAlarmRes, pObj);
        dealMsg.addMessageFunction("ReportAlarmInfo", &XMLMessageParse::ReportAlarmInfo, pObj);
        dealMsg.addMessageFunction("ControlPTZ", &XMLMessageParse::ControlPTZ, pObj);
        dealMsg.addMessageFunction("ResTransOrder", &XMLMessageParse::ResTransOrder, pObj);
        dealMsg.addMessageFunction("ResChangeOrder", &XMLMessageParse::ResChangeOrder, pObj);

        dealMsg.addMessageFunction("INFO", &XMLMessageParse::INFO, pObj);
        dealMsg.addMessageFunction("HisInfo", &XMLMessageParse::HisInfo, pObj);
        dealMsg.addMessageFunction("HisLoadInfo", &XMLMessageParse::HisLoadInfo, pObj);
    }

}


/**
	Connect to a host on a certain port number
*/
bool TCP_Client::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1) {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1) {
            perror("Could not create socket");
        }

        cout<<"Socket created\n";
    } else	{
        /* OK , nothing */
    }

    //setup address structure
    if(inet_addr(address.c_str()) == -1) {
        struct hostent *he;
        struct in_addr **addr_list;
        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL) {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return false;
        }

        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++) {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    }

    //plain ip address
    else {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }

    cout<<"Connected\n";
    register_xmlparese();

    return true;
}

/**
	Send data to the connected host
*/
bool TCP_Client::send_data(const char* data,  int numBytes)
{
    //Send some data
    if( send(sock , data, numBytes , 0) < 0) {
        perror("Send failed : ");
        return false;
    }
    return true;
}

/**
	Receive data from the connected host
*/
string TCP_Client::receive(int size=512)
{
    char buffer[size];
    string reply;
    memset(buffer, '\0',  size);

    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0) {
        //puts("recv failed");
    }
    string sx_msg;
    const char *d = "||";
    char *p;
    p = strtok(buffer,d);
    while(p) {
        sx_msg = p;
        if(sx_msg.find("INVITE") == string::npos) { //响应消息，忽略即可, 暂时先打印以供调试
            if(sx_msg.length() != 0) {
                std::cout<<"+++ TCP_Client.cpp-recive()--reply message from server: ---"<<std::endl<<sx_msg;
            }
        } else { //服务器请求消息----- //处理
            std::cout<<"----------   server request msg:  ------------"<<std::endl<<sx_msg;
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
            send_data(send_sx_msg.c_str(), send_sx_msg.length());
        }

        p=strtok(NULL,d);
    }
    /*******************************************************************************************/
    reply = buffer;
    return reply;
}

