#include"sql_conn_pool.h"
using namespace std;

SQLConnPool::SQLConnPool()
{
    m_usedConn = 0;
    m_freeConn = 0;
}
SQLConnPool::~SQLConnPool()
{
    destroyPool();
}
SQLConnPool *SQLConnPool::getInstance()
{
    static SQLConnPool connPool;
    return &connPool;
}
void SQLConnPool::init(string url, string user, string pwd, string DBName, int port, int maxConn)
{
    m_url = url;
    m_user = user;
    m_pwd = pwd;
    m_DBName = DBName;
    m_port = port;
    m_maxConn = maxConn;
    for(int i=0;i<maxConn;i++){
        MYSQL* conn = NULL;
        conn = mysql_init(conn);
        if(conn==NULL){
            LOG_ERROR("MySQL error.");
            exit(1);
        }
        conn = mysql_real_connect(conn,url.c_str(),user.c_str(),pwd.c_str(),DBName.c_str(),port,NULL,0);
        if(conn==NULL){
            LOG_ERROR("MySQL error.");
            exit(1);
        }
        // cout<<"create conn "<<conn<<endl;
        connList.push_back(conn);
        m_freeConn++;
    }
    sem = Sem(m_freeConn);
    m_maxConn = m_freeConn;
}

MYSQL *SQLConnPool::getConn()
{
    MYSQL* conn = NULL;
    if(connList.size() == 0)
        return NULL;
    sem.wait();
    locker.lock();
    conn = connList.front();
    connList.pop_front();
    m_freeConn--;
    m_usedConn++;
    locker.unlock();

    // cout<<"get conn "<<conn<<endl;
    return conn;
}

bool SQLConnPool::releaseConn(MYSQL *conn)
{
    if(conn == NULL)
        return false;
    locker.lock();
    connList.push_back(conn);
    m_freeConn++;
    m_usedConn--;
    locker.unlock();
    sem.post();

    // cout<<"release conn "<<conn<<endl;
    return true;
}

int SQLConnPool::getFreeConnCnt()
{
    return this->m_freeConn;
}

void SQLConnPool::destroyPool()
{
    locker.lock();
    if(connList.size()>0){
        list<MYSQL*>::iterator it;
        for(it=connList.begin();it!=connList.end();++it){
            MYSQL* conn = *it;
            mysql_close(conn);
        }
        m_usedConn = 0;
        m_freeConn = 0;
        connList.clear();
    }
    locker.unlock();
    // cout<<"destroyPool"<<endl;
}

SQLConnRAII::SQLConnRAII(MYSQL** sql,SQLConnPool* connPool){
    *sql = connPool->getConn();
    connRAII = *sql;
    pollRAII = connPool;
}
SQLConnRAII::~SQLConnRAII(){
    pollRAII->releaseConn(connRAII);
}