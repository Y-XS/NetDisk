#include "http_conn.h"
#include<assert.h>

int HttpConn::userCnt;
string HttpConn::rootDir;

HttpConn::HttpConn()
{
    m_sockFd = 0;
    m_state = STATE::READ;
    // m_res_root = new char[100];
    // strcpy(m_res_root,rootDir.c_str());

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
void HttpConn::unMap()
{
    if (m_file_address)
    {
        munmap(m_file_address, m_file_stat.st_size);
        m_file_address = nullptr;
    }
}

ssize_t HttpConn::readBuf()
{
    memset(m_readBuf, '\0', BUF_SIZE);
    return read(m_sockFd, m_readBuf, BUF_SIZE);
}

string HttpConn::doRequest()
{
    // string homePage = "<!DOCTYPE html><html lang=zh-CN><body><h1>Home Page</h1>";
    // homePage.append("<h2>用户在线数：")
    //         .append(to_string(userCnt))
    //         .append("</h2>")
    //         .append("</body></html>");
    // string errorPage = "<!DOCTYPE html><html lang=zh-CN><body><h1 style=\"color: red;\">Error Page</h1></body></html>";
    // cout<<m_request.getUrl()<<m_request.getUrl().length()<<m_request.getUrl().compare("/")<<endl;

    string file_name;
    char res_root[200];
    strcpy(res_root,rootDir.c_str());
    string url = m_request.getUrl();
    if(url.compare("/")==0){
        file_name = "/login.html";
    }else if(url.compare("/login")==0){
        file_name = "/home.html";
    }else{
        file_name = "/error.html";
    }
    memset(m_file_name,'\0',FILENAME_LEN);
    strcpy(m_file_name,strcat(res_root,file_name.c_str()));
    cout<<"request file="<<m_file_name<<endl;
    int fd = open(m_file_name, O_RDONLY);
    if(fd<0){
        Loger::getInstance()->Error("open file error!");
    }
    stat(m_file_name, &m_file_stat);
    m_file_address = (char*)mmap(0,m_file_stat.st_size,PROT_READ,MAP_PRIVATE, fd, 0);
    if(m_file_address == MAP_FAILED){
        Loger::getInstance()->Error("mmap error!");
    }
    close(fd);
    return file_name;
}
int HttpConn::doResponse(string fileName){
    m_response.clearBuf();
    //响应头
    m_response.setRequest(m_request);
    m_response.addStatusLine(200,"OK");
    m_response.addHeaders(m_file_stat.st_size);
    string result = m_response.getResponse();
    m_iov[0].iov_base = (char*)result.c_str();
    m_iov[0].iov_len = result.length();
    //响应体
    m_iov[1].iov_base = m_file_address;
    m_iov[1].iov_len = m_file_stat.st_size;
    m_iovCnt = 2;
    // return write(m_sockFd, result.c_str(), result.length());
    int ret = writev(m_sockFd, m_iov, m_iovCnt);
    unMap();
    return ret;
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
        string fileName = doRequest();
        int ret = doResponse(fileName);
        // int ret = writeBuf();
        if (ret < 0)
        {
            unMap();
            close_conn();
            return false;
        }
        Epoll::getInstance()->modFd(m_sockFd, EPOLLIN);
    }
    return true;
}

void HttpConn::close_conn()
{
    std::string info;
    info.append("client ").append(getIP()).append(" quit.");
    Loger::getInstance()->Info(info);
    if (m_sockFd != -1)
    {
        Epoll::getInstance()->delFd(m_sockFd);
        close(m_sockFd);
        m_sockFd = -1;
        userCnt--;
    }
}
