#include <memory>
#include "./server/server.h"
#include "./utils/loger.h"
#include "./http/http_response.h"

#include"test.h"

// #define _TEST

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

#ifndef _TEST
    // //启动服务器
    Server server(9999);
    server.init(8,"localhost","root","root","netdisk");
    server.start();
#else
    Test t;
    t.test();
#endif

    return 0;
}
