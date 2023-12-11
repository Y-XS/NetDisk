#include "http_conn.h"

int HttpConn::m_userCnt = 0;

HttpConn::HttpConn(){
    m_sockFd = 0;
    m_state = STATE::READ;
    memset(&m_addr,0,sizeof(m_addr));
    memset(m_readBuf,'\0',BUF_SIZE);
    memset(m_writeBuf,'\0',BUF_SIZE);
}

void HttpConn::init(int sockfd, const sockaddr_in &addr){
    this->m_sockFd = sockfd;
    this->m_addr = addr;
}

ssize_t HttpConn::readBuf()
{
    int n = read(m_sockFd,m_readBuf,1024);
    // std::cout<<"read: "<<m_readBuf<<std::endl;
    return n;
}

ssize_t HttpConn::writeBuf()
{
    return ssize_t();
}

bool HttpConn::process()
{
    //读
    if(m_state == STATE::READ){
        readBuf();
        m_request.parse(m_readBuf);
    }
    //写
    else if(m_state == STATE::WRITE){
        writeBuf();
    }
    //modfd
    return false;
}

void HttpConn::close()
{
}
