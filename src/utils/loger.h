#ifndef LOGER_H
#define LOGER_H
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include<string.h>

using namespace std;

class Loger
{
public:
    static Loger *getInstance() { return m_instance; }
    enum LOG_LEVEL
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    enum LOG_TARGET
    {
        TARGET_FILE,
        TARGET_TERMINAL,
        TARGET_FILE_AND_TERMINAL
    };
    Loger();
    Loger(Loger *);
    Loger(LOG_TARGET target, const char *path, int logKeepDays = 30);
    ~Loger();

    void setTarget(LOG_TARGET target);
    void setPath(const char *path);
    void setKeepDays(int keepDays);

    void Trace(const string &text);
    void Debug(const string &text);
    void Info(const string &text);
    void Warn(const string &text);
    void Error(const string &text);
    void Fatal(const string &text);
    static string GetCurDay();
    void test();

private:
    static Loger *m_instance;
    const size_t m_maxFolderLen = 256;
    const int m_maxFileNameLen = 64;
    string m_logPath;
    string m_logFileName;
    FILE *m_file;
    ofstream m_outfile;
    LOG_TARGET m_target;
    int m_logKeepDays = 0;

    string _GetFormatTime();
    void _HandleOldLogs();
    void _ToFile(string text);
    void _Output(const string &text, LOG_LEVEL level);
};

#endif