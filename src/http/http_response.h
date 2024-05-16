#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <stdarg.h>
#include <string>
#include "http_request.h"

class HttpResponse
{
public:
    static const int BUF_SIZE = 1024;

    HttpResponse();
    void setRequest(HttpRequest request){m_request=request;}
    bool addStatusLine(int status, const char *title);
    bool addHeaders(int content_len);
    bool addContent(const char *content);
    bool makeResponse(const char *format, ...);
    std::string getResponse(){return std::string(m_writeBuf);}
    bool clearBuf();
private:
    char m_writeBuf[BUF_SIZE];
    int m_writeIdx;
    HttpRequest m_request;
};

#endif
