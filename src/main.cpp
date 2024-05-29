#include <memory>
#include "./server/server.h"
#include "loger.h"
#include "./http/http_response.h"

#include "./utils/mysql.h"
#include "./helper/sql_conn_pool.h"

void test();

int main()
{
    //日志配置项
    Loger* loger = Loger::getInstance();
    loger->setKeepDays(30);
    loger->setTarget(Loger::LOG_TARGET::TARGET_FILE_AND_TERMINAL);
    chdir("..");
    char* dir = getcwd(nullptr,100);
    strcat(dir, "/logs");
    loger->setPath(dir);
    LOG_DEBUG("logger test");

    //启动服务器
    Server server(9999);
    server.init(8,"localhost","root","root","webserver");
    server.start();

    return 0;
}

void test()
{
    SQLConnPool *connPool;
    connPool = SQLConnPool::getInstance();
    string url = "localhost";
    int port = 3306;
    string user = "root";
    string pwd = "root";
    string DBName = "webserver";
    connPool->init(url, user, pwd, DBName, port, 8);

    MYSQL *mysql = NULL;
    SQLConnRAII mysqlConn(&mysql, connPool);
    string sql = "select * from users";
    mysql_query(mysql, sql.c_str());
    MYSQL_RES *res = mysql_store_result(mysql);
    int nums = mysql_num_fields(res);
    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    while (MYSQL_ROW row = mysql_fetch_row(res))
    {
        for (int i = 0; i < nums; i++)
        {
            cout << row[i] << "|";
        }
        cout << endl;
    }
}
