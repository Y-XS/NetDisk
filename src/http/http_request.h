#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include<iostream>
#include<unordered_map>
#include<vector>
#include <ctype.h>
#include<regex>
#include<string.h>
#include"../utils/loger.h"

#define CR '\r'
#define LF '\n'
enum class METHOD{
    GET,
    POST,
    HEAD,
    PUT,
    DELETE,
    TRACE,
    OPTIONS,
    CONNETC,
    PATCH
};
enum class PARSE_STATE{
    START,
    REQUESTLINE,
    HEADER,
    BODY,
    COMPLEIE,

    INVALID,
    INVALID_METHOD,
    INVALID_URI,
    INVALID_VERSION,
    INVALID_HEADER,
};

class HttpRequest{
public:
    HttpRequest();
    ~HttpRequest();
    void parse(std::string requestStr);
    std::string getMethod() {return method2str(m_method);}
    std::string getUrl() {return m_url;}
    std::unordered_map<std::string, std::string> getGetParams() {return m_get_params;}
    std::unordered_map<std::string, std::string> getPostParams() {return m_post_params;}
    std::string getProtocol() {return m_protocol;}
    std::string getVersion() {return m_version;}
    std::unordered_map<std::string, std::string>& getHeaders() {return m_headers;}
    std::string getBody() {return m_body;}

private:
    PARSE_STATE m_state;
    //请求行参数
    METHOD m_method;
    std::string m_url;
    std::unordered_map<std::string, std::string> m_get_params;
    std::string m_protocol;
    std::string m_version;
    //请求头参数
    std::unordered_map<std::string,std::string> m_headers;
    //请求体
    std::string m_body;
    std::unordered_map<std::string, std::string> m_post_params;
    int m_bodySize;
    int m_nextPos;

    std::string method2str(METHOD method);
    METHOD str2method(std::string str);
};

#endif