#include "loger.h"
#include <vector>
#include <stdarg.h>

// Loger* Loger::m_instance = new Loger(Loger::TARGET_TERMINAL,"../../logs");
Loger *Loger::m_instance = new Loger;

Loger::Loger()
{
    m_file = nullptr;
    m_logPath = "";
    m_logFileName = "";
    m_file = nullptr;
}

void Loger::setTarget(LOG_TARGET target) { this->m_target = target; }
void Loger::setPath(const char *path)
{
    if (strlen(path) > m_maxFolderLen)
    {
        cout << "日志目录过长" << endl;
        return;
    }
    if (m_target != TARGET_TERMINAL && access(path, 0) != 0)
    {
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    this->m_logPath = path;
}
void Loger::setKeepDays(int keepDays) { this->m_logKeepDays = keepDays; }

string Loger::_GetFormatTime()
{
    time_t curTime;
    time(&curTime);
    tm *tm = localtime(&curTime);

    char formatTime[64] = {0};
    snprintf(formatTime, 64, "[%04d-%02d-%02d %02d:%02d:%02d]",
             tm->tm_year + 1900,
             tm->tm_mon + 1,
             tm->tm_mday,
             tm->tm_hour,
             tm->tm_min,
             tm->tm_sec);
    return string(formatTime);
}
void Loger::_HandleOldLogs()
{
    DIR *dir = opendir(m_logPath.c_str());
    struct dirent *dp;
    vector<string> fileNames;
    string fileName;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            fileName = dp->d_name;
            fileNames.push_back(fileName.substr(0, 10));
        }
    }
    cout << fileNames.front() << endl;
    tm tm_log;
    strptime(fileNames.front().c_str(), "%Y-%m-%d", &tm_log);
    tm_log.tm_year += 1900;
    tm_log.tm_mon += 1;
    cout << tm_log.tm_year << " " << tm_log.tm_mon << " " << tm_log.tm_mday << endl;

    time_t curTime;
    time(&curTime);
    tm *tm_cur = localtime(&curTime);

    closedir(dir);
}

void Loger::_ToFile(string text)
{
    m_logFileName = GetCurDay().append(".log");
    string logPath = string(m_logPath).append("/").append(m_logFileName);
    m_outfile.open(logPath, ios::app);
    if (!m_outfile)
    {
        cerr << "[ERROR]:" << "open file failed" << endl;
        return;
    }
    m_outfile << text;
    m_outfile.close();
}

void Loger::writeLog(int level, const char *format, ...)
{
    string prefix;
    switch (level)
    {
    case TRACE:
        prefix = "[TRACE]";
        break;
    case DEBUG:
        prefix = "[DEBUG]";
        break;
    case INFO:
        prefix = "[INFO]";
        break;
    case WARN:
        prefix = "[WARN]";
        break;
    case ERROR:
        prefix = "[ERROR]";
        break;
    case FATAL:
        prefix = "[FATAL]";
        break;
    default:
        prefix = "[DEBUG]";
        break;
    }
    char buf[1024];
    va_list valist;
    va_start(valist, format);
    vsnprintf(buf, 1024, format, valist);
    string text(buf);
    string logItem;
    logItem.append(prefix)
        .append(_GetFormatTime())
        .append(":")
        .append(text)
        .append("\n");
    switch (m_target)
    {
    case TARGET_TERMINAL:
        cout << logItem;
        break;
    case TARGET_FILE:
    {
        _ToFile(logItem);
        break;
    }
    case TARGET_FILE_AND_TERMINAL:
    {
        cout << logItem;
        _ToFile(logItem);
        break;
    }
    default:
        cout << logItem;
        break;
    }
}

string Loger::GetCurDay()
{
    time_t curTime;
    time(&curTime);
    tm *tm = localtime(&curTime);
    char formatTime[32] = {0};
    snprintf(formatTime, 32, "%04d-%02d-%02d",
             tm->tm_year + 1900,
             tm->tm_mon + 1,
             tm->tm_mday);
    return string(formatTime);
}

void Loger::test()
{
    _HandleOldLogs();
}

// Loger::~Loger()
// {
// }
