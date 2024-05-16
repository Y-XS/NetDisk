#include "http_conn.h"
#include<assert.h>
#include<dirent.h>

int HttpConn::userCnt;
string HttpConn::rootDir;

HttpConn::HttpConn()
{
    m_sockFd = 0;
    m_state = STATE::READ;
    m_writeInfo_len = 0;
    bytes_to_send = 0;
    bytes_have_send = 0;
    m_file_address = nullptr;
    
    memset(&m_addr, 0, sizeof(m_addr));
    memset(m_readBuf, '\0', BUF_SIZE);
    memset(m_writeBuf, '\0', BUF_SIZE);
    memset(m_resp_header, '\0', HEADER_SIZE);
    memset(m_file_name, '\0', FILENAME_LEN);
}
void HttpConn::getFileList(){
    // cout<<"*****getFileList*****"<<endl;
    // DIR* pDir;
    // struct dirent* ptr;
    // if(!(pDir = opendir("./res"))){
    //     Loger::getInstance()->Debug("Folder doesn't exist!");
    //     return;
    // }
    // while((ptr = readdir(pDir))!=0){
    //     if(strcmp(ptr->d_name,".")!=0 && strcmp(ptr->d_name,"..")!=0){
    //         m_file_set.insert(ptr->d_name);
    //     }
    // }
    // for(auto file:m_file_set){
    //     cout<<file<<endl;
    // }
    // closedir(pDir);
}
void HttpConn::init(int sockfd, const sockaddr_in &addr)
{
    // getFileList();
    this->m_sockFd = sockfd;
    this->m_addr = addr;
    userCnt++;
}
void HttpConn::init(){
    // m_sockFd = 0;
    // m_state = STATE::READ;
    // m_writeInfo_len = 0;
    // m_file_address = nullptr;
    // bytes_to_send = 0;
    // bytes_have_send = 0;
    // memset(&m_addr, 0, sizeof(m_addr));
    // memset(m_readBuf, '\0', BUF_SIZE);
    // memset(m_writeBuf, '\0', BUF_SIZE);
    // memset(m_resp_header, '\0', BUF_SIZE);
    // memset(m_file_name, '\0', FILENAME_LEN);
}
void HttpConn::unmap()
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
int HttpConn::doGet(){
    string file_name;
    char res_root[200];
    strcpy(res_root,rootDir.c_str());
    if(m_request.getUrl().compare("/")==0){
        file_name = "/index.html";
    }else{
        file_name = m_request.getUrl();
        // if(m_file_set.count(file_name.substr(1,file_name.size()))<=0){
        //     file_name = "/error.html";
        // }
    }
    memset(m_file_name,'\0',FILENAME_LEN);
    strcpy(m_file_name,strcat(res_root,file_name.c_str()));
    // cout<<m_file_name<<endl;
    int fd = open(m_file_name, O_RDONLY);
    if(fd<0){
        Loger::getInstance()->Error("open file error!");
        strcpy(res_root,rootDir.c_str());
        strcpy(m_file_name,strcat(res_root,"/404.html"));
        fd = open(m_file_name, O_RDONLY);
    }
    stat(m_file_name, &m_file_stat);
    m_file_address = (char*)mmap(0,m_file_stat.st_size,PROT_READ,MAP_PRIVATE, fd, 0);
    if(m_file_address == MAP_FAILED){
        Loger::getInstance()->Error("mmap error!");
    }
    cout<<fd<<" "<<m_file_name<<" "<<m_file_stat.st_size<<endl;
    close(fd);
    return 0;
}
int HttpConn::doPost(){
    string url = m_request.getUrl();
    string body = m_request.getBody();
    string retMsg;
    if(url.compare("/login")==0){
        //登录逻辑
        std::unordered_map<std::string, std::string> map = m_request.getPostParams();
        if(map["userName"].compare("admin")==0 && map["password"].compare("123")==0){
            cout<<"账号密码正确"<<endl;
            retMsg = "登录成功！";
        }else{
            cout<<"用户名或密码错误"<<endl;
            retMsg = "用户名或密码错误";
        }
        strcpy(m_writeBuf,retMsg.c_str());
        m_writeInfo_len = retMsg.length();
    }
    else if(url.compare("/register")==0){
        //注册逻辑
    }
}

int HttpConn::doRequest()
{
    string method = m_request.getMethod();
    if(method.compare("GET")==0){
        doGet();
        return 0;
    }
    else if(method.compare("POST")==0){
        doPost();
        return 0;
    }
    return 0;
}
bool HttpConn::doResponse(){
    std::string method = m_request.getMethod();
    if(method.compare("GET")==0){
        m_response.clearBuf();
        //响应头
        m_response.setRequest(m_request);
        m_response.addStatusLine(200,"OK");
        m_response.addHeaders(m_file_stat.st_size);

        memset(m_resp_header,'\0',HEADER_SIZE);
        strcpy(m_resp_header,m_response.getResponse().c_str());

        m_iov[0].iov_base = m_resp_header;
        m_iov[0].iov_len = strlen(m_resp_header);
        //响应体
        m_iov[1].iov_base = m_file_address;
        m_iov[1].iov_len = m_file_stat.st_size;
        m_iovCnt = 2;
        bytes_to_send = getToWriteBytes();
        return true;
    }
    else if(method.compare("POST")==0){
        m_response.clearBuf();
        //响应头
        m_response.setRequest(m_request);
        m_response.addStatusLine(200,"OK");
        m_response.addHeaders(m_writeInfo_len);
        memset(m_resp_header,'\0',HEADER_SIZE);
        strcpy(m_resp_header,m_response.getResponse().c_str());
        m_iov[0].iov_base = m_resp_header;
        m_iov[0].iov_len = strlen(m_resp_header);
        //响应体
        m_iov[1].iov_base = m_writeBuf;
        m_iov[1].iov_len = m_writeInfo_len;
        m_iovCnt = 2;
        bytes_to_send = getToWriteBytes();
        return true;
    }
}

bool HttpConn::writeBuf(){
    int temp = 0;
    if(bytes_to_send == 0){
        Epoll::getInstance()->modFd(m_sockFd, EPOLLIN);
        init();
        return true;
    }
    while(1){
        temp = writev(m_sockFd, m_iov, m_iovCnt);
        cout<<"to_send\thave_send\tsend_len\n"
            <<bytes_to_send<<"\t"
            <<bytes_have_send<<"\t"
            <<temp<<endl;

        if(temp < 0){
            // cout<<"errno="<<errno<<" "<<strerror(errno)<<endl;
            if(errno == EAGAIN){
                // cout<<"errno=EAGAIN"<<endl;
                Epoll::getInstance()->modFd(m_sockFd, EPOLLOUT);
                return true;
            }
            unmap();
            return false;
        }
        bytes_have_send += temp;
        bytes_to_send -= temp;
        //iov[0] 发送完毕
        if(bytes_have_send >= m_iov[0].iov_len){
            m_iov[0].iov_len = 0;
            m_iov[1].iov_base = m_file_address + (bytes_have_send - strlen(m_resp_header));
            m_iov[1].iov_len = bytes_to_send;
        }
        //iov[0]未发送完毕
        else{
            m_iov[0].iov_base = m_writeBuf + bytes_have_send;
            m_iov[0].iov_len = m_iov[0].iov_len - bytes_have_send;
        }
        //发送完毕
        if(bytes_to_send <= 0){
            unmap();
            bytes_have_send = 0;
            Epoll::getInstance()->modFd(m_sockFd, EPOLLIN);
            return true;
        }
    }
    return true;
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
            return false;
        }
        else
        {
            m_request.parse(m_readBuf);
            // 处理请求
            doRequest();
            // 构造响应；
            doResponse();
            Epoll::getInstance()->modFd(m_sockFd, EPOLLOUT);
        }
    }
    // 写
    else if (m_state == STATE::WRITE)
    {
        Loger::getInstance()->Debug("STATE::WRITE");
        bool ret = writeBuf();
        if (!ret)
        {
            // unmap();
            close_conn();
            return false;
        }
    }
    return true;
}

void HttpConn::close_conn()
{
    if (m_sockFd != -1)
    {
        std::string info;
        info.append("client ").append(getIP()).append(" quit.");
        Loger::getInstance()->Info(info);

        Epoll::getInstance()->delFd(m_sockFd);
        close(m_sockFd);
        m_sockFd = -1;
        userCnt--;
    }
}
