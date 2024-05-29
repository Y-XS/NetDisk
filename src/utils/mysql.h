#ifndef MYSQL_H
#define MYSQL_H
#include <mysql/mysql.h>
#include <string>
#include <iostream>
using namespace std;

//MySQL示例
class MysqlUtil
{
public:
    void test()
    {
        // 初始化mysql连接
        MYSQL *conn = nullptr;
        conn = mysql_init(conn);
        string url = "localhost";
        int port = 3306;
        string user = "root";
        string pwd = "root";
        string DBName = "webserver";
        conn = mysql_real_connect(conn, url.c_str(), user.c_str(), pwd.c_str(), DBName.c_str(), port, NULL, 0);
        // 执行sql语句
        string sql = "insert into `users`(`username`,`password`,`desc`) values ('admin','admin','administrator');";
        string sql2 = "insert into `users`(`username`,`password`,`desc`) values ('alex','123','first user');";
        mysql_query(conn, sql.c_str());
        mysql_query(conn, sql2.c_str());
        mysql_query(conn, "select * from users");
        // 处理sql结果
        MYSQL_RES *res;
        MYSQL_ROW row;
        res = mysql_use_result(conn);
        const char *charset = "utf-8";
        mysql_set_character_set(conn, charset);
        // 列数
        int nums = 0;
        nums = mysql_num_fields(res);
        cout << nums << endl;
        // 列名
        MYSQL_FIELD *fields;
        fields = mysql_fetch_field(res);
        for (int i = 0; i < nums; i++)
        {
            cout << fields[i].name << "|";
        }
        cout << endl;
        // 每行数据
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            for (int i = 0; i < nums; i++)
            {
                cout << row[i] << "|";
            }
            cout << endl;
        }

        mysql_free_result(res);
        mysql_close(conn);
    }
};

#endif