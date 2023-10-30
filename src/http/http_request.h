#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include<iostream>
using namespace std;

class HttpRequest{
public:
    void parse();
private:
    /*所有socket上的事件都被注册到同一个epoll内核事件表中，所以将epoll文件描述符设置为静态的*/
    // static int m_epollfd;
    // int m_sockfd;
    
    /*HTTP请求方法，但我们仅支持GET*/
    enum METHOD{GET=0,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNETC,PATCH};
    /*解析客户请求时，主状态机所处的状态（回忆第8章）*/
    enum CHECK_STATE{STATE_REQUESTLINE=0,STATE_HEADER,STATE_CONTENT};
    /*行的读取状态*/
    enum LINE_STATUS{LINE_OK=0,LINE_BAD,LINE_OPEN};


    //请求行参数
    METHOD m_method;
    char* m_url;
    char* m_version;
    //请求头参数
    char* m_host;
    int m_content_length;

};

#endif