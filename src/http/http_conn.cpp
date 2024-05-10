#include "http_conn.h"

int HttpConn::userCnt;
string HttpConn::rootDir;

HttpConn::HttpConn()
{
    m_sockFd = 0;
    m_state = STATE::READ;
    memset(&m_addr, 0, sizeof(m_addr));
    memset(m_readBuf, '\0', BUF_SIZE);
    memset(m_writeBuf, '\0', BUF_SIZE);
}

void HttpConn::init(int sockfd, const sockaddr_in &addr)
{
    this->m_sockFd = sockfd;
    this->m_addr = addr;
    userCnt++;
}

ssize_t HttpConn::readBuf()
{
    memset(m_readBuf, '\0', BUF_SIZE);
    return read(m_sockFd, m_readBuf, BUF_SIZE);
}

ssize_t HttpConn::writeBuf()
{

    // string str = "server result.";
    string str = "<!DOCTYPE html><html lang=zh-CN><body><h1>Home Page</h1></body></html>";
    string result;
    std::cout << "str.size() = " << str.size() << std::endl;

    result.append("HTTP/1.1 200 OK\r\n")
        .append("Content-Type:text/html\r\n")
        .append("Connection:keep-alive\r\n")
        .append("Content-Length:" + std::to_string(str.size()))
        .append("\r\n\r\n")
        .append(str);
    Loger::getInstance()->Debug("=====result=====");
    Loger::getInstance()->Debug(result);
    return write(m_sockFd, result.c_str(), result.length());
}
string HttpConn::doRequest()
{
    string homePage = "<!DOCTYPE html><html lang=zh-CN><body><h1>Home Page</h1>";
    homePage.append("<h2>用户在线数：")
            .append(to_string(userCnt))
            .append("</h2>")
            .append("</body></html>");
    string errorPage = "<!DOCTYPE html><html lang=zh-CN><body><h1 style=\"color: red;\">Error Page</h1></body></html>";
    cout<<m_request.getUrl()<<m_request.getUrl().length()<<m_request.getUrl().compare("/")<<endl;
    if(m_request.getUrl().compare("/")==0){
        return homePage;
    }else{
        return errorPage;
    }
}
int HttpConn::doResponse(string retBody){
    string result;
    result.append("HTTP/1.1 200 OK\r\n")
        .append("Content-Type:text/html\r\n")
        .append("Connection:keep-alive\r\n")
        .append("Content-Length:" + std::to_string(retBody.size()))
        .append("\r\n\r\n")
        .append(retBody);
    return write(m_sockFd, result.c_str(), result.length());
}


bool HttpConn::process()
{
    // 读
    if (m_state == STATE::READ)
    {
        Loger::getInstance()->Debug("STATE::READ");
        int ret = readBuf();
        if (ret <= 0 && errno != EAGAIN)
        {
            close_conn();
            return false;
        }
        else
        {
            m_request.parse(m_readBuf);
            Epoll::getInstance()->modFd(m_sockFd, EPOLLOUT);
        }
        // if(ret == 0 || ret == -1){
        //     Epoll::getInstance()->modFd(m_sockFd,EPOLLIN);
        //     return false;
        // }
    }
    // 写
    else if (m_state == STATE::WRITE)
    {
        Loger::getInstance()->Debug("STATE::WRITE");
        // 构造响应；
        string retBody = doRequest();
        int ret = doResponse(retBody);
        // int ret = writeBuf();
        if (ret < 0)
        {
            close_conn();
            return false;
        }
        Epoll::getInstance()->modFd(m_sockFd, EPOLLIN);
    }
    return true;
}

void HttpConn::close_conn()
{
    if (m_sockFd != -1)
    {
        Epoll::getInstance()->delFd(m_sockFd);
        m_sockFd = -1;
        userCnt--;
    }
}
