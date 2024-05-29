#ifndef SQL_CONN_POOL_H
#define SQL_CONN_POOL_H
#include<mysql/mysql.h>
#include<string>
#include"../utils/loger.h"
#include"locker.h"
#include<list>

class SQLConnPool
{
public:
    static SQLConnPool* getInstance();
    //获取数据库连接
    MYSQL* getConn();
    //释放连接
    bool releaseConn(MYSQL* conn);
    //获取空闲连接数
    int getFreeConnCnt();
    //销毁数据库连接池
    void destroyPool();
    void init(string url,string user,string pwd,string DBName,int port,int maxConn);

    string m_url;
    string m_port;
    string m_user;
    string m_pwd;
    string m_DBName;

private:
    SQLConnPool();
    ~SQLConnPool();
    int m_maxConn;
    int m_usedConn;
    int m_freeConn;
    Locker locker;
    list<MYSQL*> connList;
    Sem sem;
};

class SQLConnRAII
{
public:
    SQLConnRAII(MYSQL** conn,SQLConnPool* connPool);
    ~SQLConnRAII();
private:
    MYSQL* connRAII;
    SQLConnPool* pollRAII;
};

#endif