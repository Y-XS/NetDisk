#include<string>
#include<mysql/mysql.h>
#include"../helper/sql_conn_pool.h"
#include<string.h>
//登录
std::string login(std::string userName,std::string pwd){
    std::string retMsg = "用户名或密码错误";
    MYSQL* mysql = NULL;
    SQLConnRAII conn(&mysql,SQLConnPool::getInstance());
    std::string sql = "select username,password from users";
    if(mysql_query(mysql,sql.c_str()))
        LOG_ERROR("SELECT error:%s\n",mysql_error(mysql));
    MYSQL_RES* result = mysql_store_result(mysql);
    int nums = mysql_num_fields(result);
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    while(MYSQL_ROW row = mysql_fetch_row(result)){
        if(strcmp(row[0],userName.c_str())==0 && strcmp(row[1],pwd.c_str())==0)
        {
            retMsg = "登录成功！";
            break;
        }
    }
    return retMsg;
}
//注册
std::string userRegister(std::string userName,std::string pwd){
    std::string retMsg = "注册失败";
    MYSQL* mysql = NULL;
    SQLConnRAII conn(&mysql,SQLConnPool::getInstance());
    // string sql = "insert into `users`(`username`,`password`,`desc`) values ('admin','admin','administrator');";
    char sql[200];
    sprintf(sql,"insert into `users`(`username`,`password`,`desc`) values ('%s','%s','');",userName.c_str(),pwd.c_str());
    LOG_DEBUG(sql);
    if(mysql_query(mysql,sql)){
        LOG_ERROR("insert error:%s",mysql_error(mysql));
        return retMsg;
    }
    return "注册成功！";
}



