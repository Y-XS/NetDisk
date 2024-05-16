#include "server.h"
#include<errno.h>

Server::Server(int port):m_port(port){
    //资源根目录
    chdir("..");
    char* dir = getcwd(nullptr,100);
    strcat(dir, "/res");
    HttpConn::rootDir = string(dir);
    //初始化HTTP连接信息
    //http连接数组
    m_users = new HttpConn[MAX_FD];
    HttpConn::userCnt = 0;
    //线程池
    m_threadpool = new ThreadPool<HttpConn>;
    m_epoll = make_shared<Epoll>(Epoll::getInstance());
    m_loger = make_shared<Loger>(Loger::getInstance());
    //数据库连接池

    //服务器初始化
    _init();
}
Server::~Server(){
    delete m_threadpool;
    delete[] m_users;
    close(m_listenfd);
}

void Server::_init(){
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_port = htons(m_port);
    m_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = 0;
    if((m_listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        m_loger->Error("create socket error.");
    }
    //设置端口复用
    int opt = 1;
    setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if((ret = bind(m_listenfd,(struct sockaddr*)&m_serv_addr,sizeof(m_serv_addr))) == -1){
        m_loger->Error("bind error.");
    }
    if((ret = listen(m_listenfd,128)) == -1){
        m_loger->Error("listen error.");
    }
    m_epoll->addFd(m_listenfd,EPOLLIN,false);
}

void Server::start(){
    m_loger->Debug("Server start...");
    while(true){
        int event_cnt = m_epoll->wait(-1);
        if(event_cnt<0 && errno!=EINTR){
            m_loger->Error("epoll wait error.");
            break;
        }
        for(int i=0;i<event_cnt;i++){
            int sockfd = m_epoll->getEventFd(i);
            uint32_t events = m_epoll->getEvents(i);
            if(sockfd == m_listenfd){
                _handleListen();
            }else if(events & (EPOLLRDHUP|EPOLLHUP|EPOLLERR)){
                //服务端关闭连接，移除对应定时器
                _closeConn(&m_users[sockfd]);
            }else if(events & EPOLLIN){
                
                _handleRead(&m_users[sockfd]);
            }else if(events & EPOLLOUT){
                _handleWrite(&m_users[sockfd]);
            }else{
                m_loger->Error("unexpected event.");
            }
        }
        //定时器
        
    }
}

void Server::_addClient(int fd, sockaddr_in addr){
    assert(fd>0);
    m_users[fd].init(fd,addr);
    m_epoll->addFd(fd,EPOLLIN,true);
    std::string str;
    str.append("client ").append(m_users[fd].getIP()).append(" visit.");
    m_loger->Info(str.c_str());
}

void Server::_handleListen(){
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int connfd = accept(m_listenfd,(struct sockaddr*)&client_addr,&client_addr_len);
    if(connfd<0){
        m_loger->Error("accept error.");
        return;
    }
    if(HttpConn::userCnt>=MAX_FD){
        //send to client: Internal server busy
        return;
    }
    _addClient(connfd,client_addr);
}
void Server::_handleRead(HttpConn* client){
    client->setState(HttpConn::STATE::READ);
    m_threadpool->append(client);
}
void Server::_handleWrite(HttpConn* client){
    client->setState(HttpConn::STATE::WRITE);
    m_threadpool->append(client);
}

void Server::_closeConn(HttpConn* client){

    // m_epoll->delFd(client->getFd());
    client->close_conn();
}