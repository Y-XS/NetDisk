#ifndef LOGER_H
#define LOGER_H
#include<fstream>
#include<dirent.h>
#include<unistd.h>
#include<sys/stat.h>
#include<iostream>
using namespace std;

class Loger{
public:
    enum LOG_LEVEL{
        TRACE,
        DEBUG,
        INFORMATION,
        WARNING,
        ERROR,
        FATAL
    };
    enum LOG_TARGET{
        TARGET_FILE,
        TARGET_TERMINAL,
        TARGET_FILE_AND_TERMINAL
    };
    Loger(){}
    Loger(LOG_TARGET target, LOG_LEVEL level, const char* logFloder, int logKeepDays=10);
    ~Loger();

    void Trace(const string &text);
    void Debug(const string &text);
    void Information(const string &text);
    void Warn(const string &text);
    void Error(const string &text);
    void Fatal(const string &text);
    static string GetCurDay();
    void test();

private:
    const size_t maxFolderLen = 256;
    const int maxFileNameLen = 64;
    char m_logFolder[256];
    char m_logFileName[64];
    FILE* m_file;
    ofstream m_outfile;
    LOG_TARGET m_target;
    LOG_LEVEL m_level;
    int m_logKeepDays;
    
    string _GetFormatTime();
    void _HandleOldLogs();
    void _ToFile(string text);
    void _Output(const string &text, LOG_LEVEL level);
};

#endif