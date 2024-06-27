#include"netdisk.h"
#include<dirent.h>
#include<unistd.h>
#include<vector>
#include<sys/stat.h>
#include<ctime>
#include<fstream>
#include<errno.h>
#include"../utils/loger.h"
#include"../helper/sql_conn_pool.h"

NetDisk::NetDisk()
{
    // cout<<"=====NetDisk constructor====="<<endl;
    int ret = access("disk",F_OK);
    if(ret!=0){
        if(mkdir("disk",0700)!=0){
            LOG_ERROR("mkdir error");
        }
    }
    // chdir("./disk");
    char* dir = getcwd(nullptr,1024);
    m_curDir = string(dir) + "/disk";
}

string NetDisk::login(string jsonStr){
    JSON json = JSON::parse(jsonStr);
    JSON retJson;
    m_userName = json["username"];
    string pwd = json["password"];

    MYSQL* mysql = NULL;
    SQLConnRAII conn(&mysql,SQLConnPool::getInstance());
    std::string sql = "select * from users where username='"+m_userName+"'";
    if(mysql_query(mysql,sql.c_str()))
        LOG_ERROR("SELECT error:%s\n",mysql_error(mysql));
    MYSQL_RES* result = mysql_store_result(mysql);
    int nums = mysql_num_rows(result);
    if(nums==0){
        retJson["code"]=-1;
        retJson["msg"]="用户名不存在!";
        return retJson.dump();
    }
    while(MYSQL_ROW row = mysql_fetch_row(result)){
        if(strcmp(row[2],pwd.c_str())!=0){
            retJson["code"]=-2;
            retJson["msg"]="密码错误!";
            return retJson.dump();
        }else{
            retJson["code"]=0;
            retJson["msg"]="登录成功！";
            JSON userInfo;
            userInfo["id"] = row[0];
            userInfo["username"] = row[1];
            userInfo["tel"] = row[3];
            retJson["userInfo"]=userInfo;
            _checkDir(m_userName);
            return retJson.dump();
        }
    }
}
std::string NetDisk::regis(std::string jsonStr){
    return std::string();
}
std::string NetDisk::getFileList(std::string dir){
    // cout<<"=====getFileList====="<<endl;
    string path = dir;
    while(dir.find("%2F")!=string::npos){
        path = dir.replace(dir.find("%2F"),3,"/");
    }
    std::vector<string> fileList;
    JSON retJson;
    string absoluteDir = m_curDir + path;
    cout<<"absoluteDir: "+absoluteDir<<endl;

    DIR* pDir;
    struct dirent* ent;
    if(!(pDir = opendir(absoluteDir.c_str()))){
        LOG_ERROR("Folder doesn't exist!");    
        retJson["code"]=-1;
        retJson["msg"]="Folder doesn't exist!";
        return retJson.dump();
    }
    while((ent = readdir(pDir)) != NULL){
        //处理目录
        if(ent->d_type & DT_DIR){
            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
                continue;
            JSON dirInfo;
            dirInfo["type"]="dir";
            dirInfo["name"]=ent->d_name;
            dirInfo["size"]="-";
            dirInfo["mtime"]="-";
            fileList.push_back(dirInfo.dump());
        }else{
            JSON fileInfo;
            fileInfo["type"]="file";
            fileInfo["name"]=ent->d_name;
            struct stat fileStat;

            string path = absoluteDir+"/"+ent->d_name;
            LOG_DEBUG("path=%s",path.c_str());
            int ret = stat(path.c_str(),&fileStat);
            if(ret!=0)
                LOG_ERROR("stat error");
            struct tm* t = localtime(&fileStat.st_mtim.tv_sec);
            char time[64];
            strftime(time,sizeof(time),"%Y-%m-%d %H:%M", t);
            fileInfo["mtime"]=time;
            fileInfo["size"]=fileStat.st_size;
            fileList.push_back(fileInfo.dump());
        }
    }
    retJson["code"]=0;
    retJson["msg"]="获取文件列表成功";
    retJson["fileList"]=fileList;
    return retJson.dump();
}

string NetDisk::mkDIR(string dir)
{
    string path = dir;
    while(dir.find("%2F")!=string::npos){
        path = dir.replace(dir.find("%2F"),3,"/");
    }
    string absoluteDir = m_curDir + path;
    JSON retJson;
    int ret = access(absoluteDir.c_str(),F_OK);
    if(ret!=0){
        if(mkdir(absoluteDir.c_str(),0700)!=0){
            retJson["code"]=-1;
            retJson["msg"]="文件夹创建失败";
            LOG_ERROR("mkdir error");
            return retJson;
        }
    }
    retJson["code"]=0;
    retJson["msg"]="文件夹创建成功！";
    return retJson.dump();
}

string NetDisk::upload(const char* buf,int len,string dir,string fileName)
{
    JSON retJson;
    // LOG_DEBUG((m_curDir+dir+"/"+fileName).c_str());
    ofstream file(m_curDir+dir+"/"+fileName,ios::out|ios::binary);
    if(!file){
        retJson["code"]=-1;
        retJson["msg"]="文件上传失败";
        LOG_ERROR("upload file error");
        return retJson.dump();
    }
    file.write(buf,len);
    retJson["code"]=0;
    retJson["msg"]="文件上传成功！";
    return retJson.dump();
}

int NetDisk::delFile(string dir)
{
    JSON retJson;
    return unlink((m_curDir+dir).c_str());
}

string NetDisk::delDir(string dir)
{
    return string();
}

bool NetDisk::_checkDir(std::string userName)
{
    int ret = access(userName.c_str(),F_OK);
    if(ret!=0){
        if(mkdir(userName.c_str(),0700)!=0){
            LOG_ERROR("mkdir error");
            return false;
        }
    }
    return true;
}

bool NetDisk::_checkUser(std::string userName)
{
    int ret = false;
    MYSQL* mysql = NULL;
    SQLConnRAII conn(&mysql,SQLConnPool::getInstance());
    std::string sql = "select * from users where username='"+userName+"'";
    if(mysql_query(mysql,sql.c_str()))
        std::cout<<"SELECT error: "<<mysql_error(mysql)<<std::endl;
    MYSQL_RES* result = mysql_store_result(mysql);
    int nums = mysql_num_rows(result);
    return nums==0?false:true;
}
