#include "http_response.h"
#include <iostream>
#include<map>

HttpResponse::HttpResponse()
{
    m_writeIdx = 0;
}

bool HttpResponse::addStatusLine(int status, const char *title)
{
    return makeResponse("%s %d %s\r\n", "HTTP/1.1", status, title);
}

bool HttpResponse::addHeaders(int content_len)
{
    makeResponse("Content-Length:%d\r\n", content_len);
    makeResponse("Access-Control-Allow-Origin:%s\r\n", "*");
    makeResponse("Connection:%s\r\n", m_request.getHeaders()["Connection"].c_str());
    makeResponse("%s", "\r\n");
    return true;
}

bool HttpResponse::addContent(const char *content)
{
    return makeResponse("%s", content);
}

bool HttpResponse::makeResponse(const char *format, ...)
{
    if (m_writeIdx >= BUF_SIZE)
        return false;
    va_list args;
    va_start(args, format);
    int len = vsnprintf(m_writeBuf + m_writeIdx, BUF_SIZE - 1 - m_writeIdx, format, args);
    if(len>=(BUF_SIZE-1-m_writeIdx)){
        va_end(args);
        return false;
    }
    va_end(args);
    m_writeIdx += len;
    return true;
}

bool HttpResponse::clearBuf()
{
    m_writeIdx = 0;
    memset(m_writeBuf,'\0',BUF_SIZE);
    return true;
}