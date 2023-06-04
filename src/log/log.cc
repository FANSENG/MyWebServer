/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-03-30 20:55:09
 * @LastEditors: fs1n
 * @LastEditTime: 2023-06-01 02:14:23
 */
#include "log.h"

Log::Log():lineCount(0), isAsync(false), writeThread(nullptr),
                      deque_(nullptr), logLastDay(0), fp(nullptr) {}

Log::~Log(){
    if(writeThread && writeThread->joinable()){
        // 先记录完所有的日志
        while(!deque_->empty()){
            deque_->flush();
        }
        deque_->close();
        writeThread->join();
    }
    // 文件打开了
    if(fp){
        std::lock_guard<std::mutex> locker(this->mtx_);
        flush();
        fclose(fp);
    }
}

void Log::init(LogLevel level, const char* path, const char* suffix, int maxQueueCapacity){
    open = true;
    this->level = level;
    if(maxQueueCapacity > 0){
        isAsync = true;
        if(!deque_){
            std::unique_ptr<BlockQueue<std::string>> newDeque(new BlockQueue<std::string>);
            deque_ = std::move(newDeque);
            std::unique_ptr<std::thread> newThread(new std::thread(flushLogThread));
            writeThread = std::move(newThread);
        }
    }else{
        isAsync = false;
    }

    lineCount = 0;

    time_t timer = time(nullptr);
    struct tm *sysTime = localtime(&timer);
    struct tm t = *sysTime;
    this->path = path;
    this->suffix = suffix;
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s",
            path, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix);
    logLastDay = t.tm_mday;

    {
        std::lock_guard<std::mutex> locker(this->mtx_);
        buff.readAll();
        if(fp){
            flush();
            fclose(fp);
        }

        fp = fopen(fileName, "a");
        if(fp == nullptr){
            mkdir(this->path, 0777);
            fp = fopen(fileName, "a");
        }
        assert(fp != nullptr);
    }
}

Log* Log::Instance(){
    static Log instance;
    return &instance;
}

void Log::flushLogThread(){
    Log::Instance()->asyncWrite();
}

void Log::write(LogLevel level, const char* format, ...){
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList;

    // 新一天的日志 或者 日志文件满了
    if(logLastDay != t.tm_mday || (lineCount && (lineCount % MAX_LEN) == 0)){
        // 此锁的作用域被 if 包裹
        std::unique_lock<std::mutex> locker(this->mtx_);
        locker.unlock();

        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        // 新一天的日志
        if(logLastDay != t.tm_mday){
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path, tail, suffix);
            logLastDay = t.tm_mday;
            lineCount = 0;
        }
        // 当天日志文件满了，新建日志文件
        else{
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path, tail, (lineCount / MAX_LEN), suffix);
        }
        locker.lock();
        
        // 将文件在内存中的缓存写入文件中
        // 要创建新的日志文件，保证旧日志文件完整
        fflush(fp);
        fclose(fp);
        fp = fopen(newFile, "a");
        assert(fp != nullptr);
    }


    {
        std::unique_lock<std::mutex> locker(mtx_);
        lineCount++;
        int n = snprintf(buff.writePtr(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        
        buff.hasWrite(n);
        appendLogLevelTitle(level);

        va_start(vaList, format);
        int m = vsnprintf(buff.writePtr(), buff.writeableBytes(), format, vaList);
        va_end(vaList);
        buff.hasWrite(m);
        buff.Append("\n\0", 2);

        // 如果异步且队列未满，则缓存内容存入阻塞队列
        // 如果队列已满或者非异步，则直接写文件
        if(isAsync && deque_ && !deque_->full()){
            deque_->push_back(buff.readAllToString());
        }else{
            fputs(buff.readPtrConst(), fp);
        }
        buff.readAll();
    }
}

void Log::flush(){
    if(isAsync){
        deque_->flush();
    }
    fflush(fp);
}

LogLevel Log::getLevel(){
    std::lock_guard<std::mutex> locker(this->mtx_);
    return level;
}

void Log::setLevel(LogLevel level){
    std::lock_guard<std::mutex> locker(this->mtx_);
    this->level = level;
}

bool Log::isOpen(){
    std::lock_guard<std::mutex> locker(this->mtx_);
    return open;
}

void Log::appendLogLevelTitle(LogLevel level){
    switch (level)
    {
    case LogLevel::DEBUG:
        buff.Append("[debuf]: ");
        break;
    case LogLevel::INFO:
        buff.Append("[info]: ");
        break;
    case LogLevel::WARN:
        buff.Append("[warn]: ");
        break;
    case LogLevel::ERROR:
        buff.Append("[error]: ");
        break;
    default:
        buff.Append("[debuf]: ");
        break;
    }
}

void Log::asyncWrite(){
    std::string str;
    while(deque_->pop(str)){
        std::lock_guard<std::mutex> locker(this->mtx_);
        fputs(str.c_str(), fp);
    }
}