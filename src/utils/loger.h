#ifndef LOGER_H
#define LOGER_H
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include<string.h>
using namespace std;

#define LOG_DEBUG(format, ...) {Loger::getInstance()->writeLog(1, format, ##__VA_ARGS__);}
#define LOG_INFO(format, ...) {Loger::getInstance()->writeLog(2, format, ##__VA_ARGS__);}
#define LOG_WARN(format, ...) {Loger::getInstance()->writeLog(3, format, ##__VA_ARGS__);}
#define LOG_ERROR(format, ...) {Loger::getInstance()->writeLog(4, format, ##__VA_ARGS__);}

class Loger
{
private:
    static Loger *m_instance;
    Loger();

public:
    static Loger* getInstance(){return m_instance;}
    Loger(const Loger& loger) = delete;
    // Loger(Loger* loger) = delete;
    void operator = (const Loger& loger) = delete;

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
    void writeLog(int level, const char *format, ...);
    void setTarget(LOG_TARGET target);
    void setPath(const char *path);
    void setKeepDays(int keepDays);

    static string GetCurDay();
    void test();
private:
    const size_t m_maxFolderLen = 256;
    const int m_maxFileNameLen = 64;
    string m_logPath;
    string m_logFileName;
    FILE *m_file;
    ofstream m_outfile;
    LOG_TARGET m_target;
    int m_logKeepDays = 0;

    // Loger(Loger *);
    // Loger(LOG_TARGET target, const char *path, int logKeepDays = 30);
    // ~Loger();
    string _GetFormatTime();
    void _HandleOldLogs();
    void _ToFile(string text);
};

#endif