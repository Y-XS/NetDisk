#include<memory>
#include"./server/server.h"
#include"loger.h"
#include "./http/http_response.h"

int main(){
    //日志配置项
    Loger* loger = Loger::getInstance();
    loger->setKeepDays(30);
    loger->setTarget(Loger::TARGET_FILE_AND_TERMINAL);
    loger->setPath("../logs");

    loger->Debug("logger test");
    Server server(9999);
    server.start();

    return 0;
}