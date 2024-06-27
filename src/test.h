#ifndef TEST_H
#define TEST_H
#include<iostream>
#include<string>

#include "./utils/mysql.h"
#include "./helper/sql_conn_pool.h"
#include "./utils/json.hpp"
#include"./server/netdisk.h"
using json = nlohmann::json;

class Test{
public:
    void testSQL();
    void testJSON();
    void test();
};

void Test::test(){
    NetDisk nd;
    cout<<getcwd(nullptr,1024)<<endl;
    string path = "./disk/admin/123/todo.txt";
    int ret = nd.delFile(path);
    cout<<ret<<endl;
}
//--delFile
// void Test::test(){
//     NetDisk nd;
//     cout<<getcwd(nullptr,1024)<<endl;
//     string path = "./disk/admin/123/todo.txt";
//     string ret = nd.delFile(path);
//     cout<<ret<<endl;
// }

void Test::testJSON(){
    std::string str = R"({ "pi":3.14,"happy":true})";
    json ex = json::parse(str);
    std::cout<< ex.dump() <<std::endl;
}
void Test::testSQL(){
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

#endif