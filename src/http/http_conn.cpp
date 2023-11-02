#include "http_conn.h"

int HttpConn::m_userCnt = 0;

HttpConn::HttpConn(){
    m_sockFd = 0;
    m_epfd = 0;
    memset(&m_addr,0,sizeof(m_addr));
}

void HttpConn::init(int sockfd, const sockaddr_in &addr, int epfd)
{
  }

ssize_t HttpConn::read()
{
    return ssize_t();
}

ssize_t HttpConn::write()
{
    return ssize_t();
}

bool HttpConn::process()
{
    //读
    //写
    //modfd
    // Epoll::m_epfd;
    return false;
}

void HttpConn::close()
{
}
