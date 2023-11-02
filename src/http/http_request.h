#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include<iostream>
#include<unordered_map>

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

class HttpRequest{
public:
    void parse(const std::string& buf);
    const METHOD& getMethod() const {return m_method;}
    const std::string& getUrl() const {return m_url;}
    const std::unordered_map<std::string, std::string>& getParams() const {return m_params;}
    const std::string& getProtocol() const {return m_protocol;}
    const std::string& getVersion() const {return m_version;}
    const std::unordered_map<std::string, std::string>& getHeaders() const {return m_headers;}
    const std::string& getBody() const {return m_body;}
private:
    /*解析客户请求时，主状态机所处的状态（回忆第8章）*/
    enum PARSE_STATE{
        STATE_REQUESTLINE,
        STATE_HEADER,
        STATE_BODY,
        INVALID_REQUEST,
    };
    PARSE_STATE m_parseState = STATE_REQUESTLINE;
    //请求行参数
    METHOD m_method;
    std::string m_url;
    std::unordered_map<std::string, std::string> m_params;
    std::string m_protocol;
    std::string m_version;
    //请求头参数
    std::unordered_map<std::string,std::string> m_headers;
    //请求体
    std::string m_body;
};

#endif