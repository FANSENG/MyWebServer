/**
 * =========================
 * 如果是同步状态，不使用 BlockQueue，生成一个日志就写入文件中
 * 如果是异步状态，有一个线程专门负责写，生成的日志先存储到 BlockQueue 中。
 * buff只是临时存储的，BlockQueue 会稍久的存储，但最终持久化是由文件实现的
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
#include "config.h"
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

    /// @brief 初始化 Log 对象
    /// @param level 日志级别
    /// @param path 日志路径
    /// @param suffix 日志后缀
    /// @param maxQueueCapacity 阻塞队列容量 
    void init(  LogLevel level, 
                const char* path = Config::LOG_PATH.c_str(), 
                const char* suffix = Config::LOG_SUFFIX.c_str(),
                int maxQueueCapacity = Config::LOG_MAXCAPACITY);
    
    /// @brief 单例模式，获取 Log*
    /// @return Log*
    static Log* Instance();

    /// @brief logThread 异步将阻塞队列中数据写入log文件
    static void flushLogThread();

    /// @brief 写日志文件
    /// @param level 日志级别[INFO, DEBUG, WARN, ERROR]
    /// @param format 
    /// @param  
    void write(LogLevel level, const char* format, ...);

    void flush();


    /// @brief 获取日志级别
    /// @return 
    LogLevel getLevel();


    /// @brief 设置日志级别
    /// @param level 
    void setLevel(LogLevel level);

    /// @brief 判断日志文件是否打开
    /// @return 
    bool isOpen();

private:
    Log();
    void appendLogLevelTitle(LogLevel level);
    virtual ~Log();

    /// @brief  不断异步写日志，是一个单独的线程
    ///         在使用 deque_->poo(item) 时，如果阻塞队列为空会休眠
    ///         直到队列中写入数据并且通过 deque_->flush() 执行 comsumer.notice_one()
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
    std::unique_ptr<BlockQueue<std::string>> deque_;
    std::unique_ptr<std::thread> writeThread;
    std::mutex mtx_;
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