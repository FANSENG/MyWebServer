/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: 用于记录日志
 * @Date: 2023-02-27 19:10:12
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-02 10:29:20
 */
/**
 * =========================
 * 如果是同步状态，不使用 BlockQueue，生成一个信息就写入文件中
 * 如果是异步状态，有一个线程专门负责写，生成的日志先存储到 BlockQueue 中。
 * =========================
*/
#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/stat.h>
#include <string>
#include "utils/config.h"
#include "blockqueue.h"
#include "buffer.h"


enum LogLevel{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR
};

// 单例模式
class Log{
public:
    void init(  LogLevel level, 
                const std::string path = webserver::Config::LOG_PATH, 
                const std::string suffix = webserver::Config::LOG_SUFFIX,
                int maxQueueCapacity = webserver::Config::LOG_MAXCAPACITY);
    
    static Log* Instance();

    static void flushLogThread();

    void write(LogLevel level, const char* format, ...);

    void flush();

    LogLevel getLevel();

    void setLevel(LogLevel level);

    bool isOpen();

private:
    Log();
    void appendLogLevelTitle(LogLevel level);
    virtual ~Log();

    void asyncWrite();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LEN = 20000;

    const char* path;
    const char* suffix;

    int lineCount;
    int logLastDay;         // 标记最后一次记日志是此月第多少天

    bool open;

    Buffer buff;
    LogLevel level;
    bool isAsync;

    FILE* fp;
    std::unique_ptr<BlockQueue<std::string>> deque;
    std::unique_ptr<std::thread> writeThread;
    std::mutex mtx;
};

#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if (log->isOpen() && log->getLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);
#define LOG_DEBUG(format, ...) do {LOG_BASE(LogLevel::DEBUG, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(LogLevel::INFO, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(LogLevel::WARN, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(LogLevel::ERROR, format, ##__VA_ARGS__)} while(0);

#endif // LOG_H