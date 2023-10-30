#include "server.h"

Server::Server(int port):m_port(port){
    m_epoll = make_shared<Epoll>(Epoll::getInstance());
    m_threadpool = new ThreadPool<HttpConn>;
    unique_ptr<Loger> loger_temp(new Loger(Loger::TARGET_FILE,"../../logs"));
    m_loger = std::move(loger_temp);
    m_users = new HttpConn[MAX_FD];
    m_user_cnt = 0;
    _init();
}

Server::~Server(){
    delete m_threadpool;
    delete[] m_users;
    m_loger.release();
    close(m_listenfd);
}

void Server::_init(){
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_port = htons(m_port);
    m_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = 0;
    if((m_listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        m_loger->Debug("socket error.");
    }
    //设置端口复用
    int opt = 1;
    setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if((ret = bind(m_listenfd,(struct sockaddr*)&m_serv_addr,sizeof(m_serv_addr))) == -1){
        m_loger->Debug("bind error.");
    }
    if((ret = listen(m_listenfd,128)) == -1){
        m_loger->Debug("listen error.");
    }
    m_epoll->addFd(m_listenfd,EPOLLIN,false);
}

void Server::start(){
    while(true){
        int event_cnt = m_epoll->wait(-1);
        if(event_cnt<0 && errno!=EINTR){
            m_loger->Debug("epoll wait error.");
            break;
        }
        for(int i=0;i<event_cnt;i++){
            int sockfd = m_epoll->getEventFd(i);
            uint32_t events = m_epoll->getEvents(i);
            if(sockfd == m_listenfd){
                _handleListen();
            }else if(events & (EPOLLRDHUP|EPOLLHUP|EPOLLERR)){
                _closeConn(&m_users[sockfd]);
            }else if(events & EPOLLIN){
                _handleRead(&m_users[sockfd]);
            }else if(events & EPOLLOUT){
                _handleWrite(&m_users[sockfd]);
            }else{
                m_loger->Error("unexpected event.");
            }
        }
    }
}

void Server::_addClient(int fd, sockaddr_in addr){
    assert(fd>0);
    m_users[fd].init(fd,addr,m_epoll->getEpfd());
    m_epoll->addFd(fd,EPOLLIN);
    std::string str;
    str.append("client ").append(m_users[fd].getIP()).append(" visit.");
    m_loger->Info(str.c_str());
}

void Server::_handleListen(){
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int connfd = accept(m_listenfd,(struct sockaddr*)&client_addr,&client_addr_len);
    if(connfd<0){
        m_loger->Debug("accept error.");
        return;
    }
    if(HttpConn::m_userCnt>=MAX_FD){
        //send to client: Internal server busy
        return;
    }
    _addClient(connfd,client_addr);
}
void Server::_handleRead(HttpConn* client){
    m_threadpool->append(client);
}
void Server::_handleWrite(HttpConn* client){
    m_threadpool->append(client);
}
void Server::_onRead(HttpConn* client){
    // assert(client);
    // client->read();
    // uint32_t connEvent = EPOLLET;
    // if(client->process()){
    //     m_epoll->modFd(client->getFd(),connEvent | EPOLLOUT);
    // }else{
    //     m_epoll->modFd(client->getFd(),connEvent | EPOLLIN);
    // }
}
void Server::_onWrite(HttpConn* client){
    // if(!m_users[sockfd].write()){
    //     m_users[sockfd].close_conn();
    // }
}

void Server::_closeConn(HttpConn* client){
    m_loger->Info("client quit.");
    m_epoll->delFd(client->getFd());
    client->close();
}