#if 1
#include "../include/Libevent_client.h"

Libevent_client::Libevent_client():base(NULL), evtimeout(NULL)
{
}

Libevent_client::~Libevent_client()
{
    if(evtimeout != NULL)
    {
        event_free(evtimeout);
    }
    if(base != NULL)
    {
        event_base_free(base);
    }
    printf("%zd total bytes read\n", total_bytes_read);
    printf("%zd total messages read\n", total_messages_read);
    printf("%.3f average messages size\n", (double)total_bytes_read / total_messages_read);
    printf("%.3f MiB/s throughtput\n",
           (double)total_bytes_read / (timeout.tv_sec * 1024 * 1024));
}

void Libevent_client::setup(const char *ip, const unsigned short& port)
{
    /* 初始化连接地址 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    /*inet_pton 处理IPv4,将“点分十进制” －> “整数 */
    res = inet_pton(AF_INET, ip, &servaddr.sin_addr);
    if( res != 1 )
    {
        error_quit("inet_pton error");
    }

    timeout_seconds = 60;
    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;
    base = event_base_new();
    if (!base)
    {
        error_quit("Couldn't open event base");
    }
    evtimeout = evtimer_new(base, timeoutcb, base);
    evtimer_add(evtimeout, &timeout);
}

void Libevent_client::connect()
{

    /* 连接服务器并监听 */
    //bufferevent_socket_new创建基于套接字的bufferevent， 参数为-1 表示并不为此 bufferevent 设置 socket
    if ((sockbev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE)) == NULL)
    {
        error_quit("bufferevent_socket_new error");
    }
    // 建立连接,并将socket设置为非阻塞,连接成功返回 0 失败返回 -1
    res = bufferevent_socket_connect(sockbev, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if ( res < 0 )
    {
        bufferevent_free(sockbev);
        error_quit("connect error");
    }
    //为bufferevent event设置回调函数
    bufferevent_setcb(sockbev, sock_readcb, NULL, event_cb, (void*)base);
    //启用读写操作
    bufferevent_enable(sockbev, EV_READ);
    bufferevent_enable(sockbev, EV_WRITE);

    //开始事件循环
    event_base_dispatch(base);
}

void Libevent_client::send(const char* data, size_t numBytes)
{
    if(bufferevent_write(sockbev, data, numBytes) == -1)
    {
        printf("Error while sending in Connection::send()\n");
    }
}

/* 发生了致命错误，输入错误信息，退出程序 */
void Libevent_client::error_quit(const char *str)
{
    /*  perror()将上一个函数发生错误的原因输出到标准错误(stderr);
        参数 s 所指的字符串会先打印出,后面再加上错误原因字符串;
        此错误原因依照全局变量 errno 的值来决定要输出的字符串。
    */
    perror(str);
    exit(1);
}


void Libevent_client::timeoutcb(evutil_socket_t fd, short what, void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    printf("timeout\n");

    event_base_loopexit(base, NULL);
}

/* 事件回调函数，连接状态改变时回调的函数 */
void Libevent_client::event_cb(struct bufferevent *bev, short events, void *ptr)
{
    struct event_base *tbase = (struct event_base*)ptr;
    if ( events & BEV_EVENT_CONNECTED)
    {
        /* We're connected to server. Ordinarily we'd do
               something here, like start reading or writing. */
        evutil_socket_t fd = bufferevent_getfd(bev);
        int one = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
        printf("Server is connected!\n");
    }
    else    //如果连接不成功的消息，就停止事件循环
    {
        bufferevent_free(bev);
        event_base_loopbreak(tbase);
        printf("Server connect erro!! or The connect have been shutdown: %X\n", events);
    }
}

/* 服务器传信息过来了
   this callback is invoked when there is data to read on bev.
*/
void Libevent_client::sock_readcb(struct bufferevent *bev, void *ptr)
{
    struct evbuffer* inputbuf = bufferevent_get_input(bev);         //获取输入缓冲区
    char readbuf[10240];
    size_t readn = 0;
    //evbuffer_remove() 函数将复制和删除前面的inputbuf的sizeof(readbuf)个字节到内存中的data中及readbuf中。
    //如果少于datlen个字节可用，函数将复制所有的字节。失败返回的值是-1，否则返回复制的字节数
    memset(readbuf,  '\0', sizeof(readbuf));
    while( (readn = evbuffer_remove(inputbuf, &readbuf, sizeof(readbuf))) > 0)
    {
        //readbuf[readn] = '\0';
    }

    std::string sx_msg(readbuf);

    if(sx_msg.find("INVITE") == 0)  //server request message--//处理
    {
        std::cout<<"-------- server request msg-------------- "<<std::endl<<sx_msg<<std::endl;
 #if 0
        std::string send_sx_msg;
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

        if(xml_body == NULL)
        {
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
#endif
    }
    else    // server 响应消息，忽略即可
    {
        std::cout<<"----------------Reply message from server:-----------------"<<std::endl<<sx_msg<<std::endl;

    }

    //处理完毕
    ++total_messages_read;
    total_bytes_read += readn;
}

//静态变量，类外初始化，链接时才不会报错
int64_t Libevent_client::total_bytes_read = 0;
int64_t Libevent_client::total_messages_read = 0;

#endif

