#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include<iostream>
using namespace std;

class HttpRequest{
public:
    void parse();
private:
    /*HTTP请求方法，但我们仅支持GET*/
    enum METHOD{GET=0,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNETC,PATCH};

    //请求行参数
    METHOD m_method;
    char* m_url;
    char* m_version;
    //请求头参数
    char* m_host;
    int m_content_length;

};

#endif