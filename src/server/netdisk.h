#ifndef NETDISK_H
#define NETDISK_H

#include<iostream>
#include<string>
#include "../utils/json.hpp"
using JSON = nlohmann::json;
using std::cout;
using std::endl;
using std::string;

class NetDisk{
public:
    NetDisk();
    string login(string jsonStr);
    string regis(string jsonStr);
    string getFileList(string dir);
    //dir 传绝对路径
    string mkDIR(string dir);
    string upload(const char* buf,int len,string dir,string fileName);
    //{dir:"",type:""}
    int delFile(string dir);
    string delDir(string dir);
    //{dir:"",type:""}
    // string rename();
private:
    string m_userName;
    string m_curDir;

    bool _checkDir(string userName);
    bool _checkUser(string userName);
};


#endif