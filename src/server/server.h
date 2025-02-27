#ifndef SERVER_H
#define SERVER_H
#include<iostream>
#include<memory>
#include<arpa/inet.h>
#include<assert.h>
#include"../helper/epoll.h"
#include"../helper/threadpool.h"
#include"../http/http_conn.h"
#include"../utils/loger.h"
#include"../helper/sql_conn_pool.h"
using namespace std;

// #define MAX_FD 65535
#define MAX_FD 20
class Server{
public:
    Server(int port);
    ~Server();

    void start();
    void init(int sqlConnNum,string url,string user,string pwd,string DBName);
private:
    int m_port;
    int m_listenfd;
    struct sockaddr_in m_serv_addr;
    HttpConn* m_users;
    // int m_user_cnt;

    std::shared_ptr<Epoll> m_epoll;
    ThreadPool<HttpConn> *m_threadpool;
    // ConnectionPool* m_connPool;
    
    void _init();
    void _addClient(int fd, sockaddr_in addr);
    void _handleListen();
    void _handleRead(HttpConn* client);
    void _handleWrite(HttpConn* client);
    void _closeConn(HttpConn* client);
};

#endif