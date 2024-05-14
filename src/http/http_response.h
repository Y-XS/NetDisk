#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <stdarg.h>
#include <string>
#include "http_request.h"

class HttpResponse
{
public:
    HttpResponse();
    void setRequest(HttpRequest request){m_request=request;}
    bool addStatusLine(int status, const char *title);
    bool addHeaders(int content_len);
    bool addContent(const char *content);
    bool makeResponse(const char *format, ...);
    std::string getResponse(){return std::string(m_writeBuf);}
    bool clearBuf();
private:
    static const int BUF_SIZE = 4096;
    char m_writeBuf[BUF_SIZE];
    int m_writeIdx;
    HttpRequest m_request;
};

#endif
