/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-02 10:15:32
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-06 11:55:35
 */

#include "sqlconnpool.h"

SqlConnPool* SqlConnPool::Instance(){
    static SqlConnPool sqlpool;
    return &sqlpool;
}

void SqlConnPool::init(const char *host, int port, const char *user, const char *psw, const char *dbName,
                       int connSize) {
    assert(connSize > 0);
    // std::cout << "Sql connSize is " << connSize << std::endl;
    // std::cout << "host: " << host << "\tuser: " << user << "\tpsw:" << psw << "\tdbName:" << dbName << "\tport:" << port << std::endl;
    for(int i = 0; i < connSize; i++){
        MYSQL* sql = nullptr;
        sql = mysql_init(sql);
        
        if(!sql){
            LOG_ERROR("Mysql init Error");
            assert(sql);
        }
        sql = mysql_real_connect(sql, host, user, psw, dbName, port, nullptr, 0);
        if(!sql){
            LOG_ERROR("Mysql Conn Error");
            continue;
        }
        pool_.push(sql);
    }
    MAXCONN = pool_.size();
    sem_init(&sqlPoolSem, 0, MAXCONN);
}

MYSQL* SqlConnPool::getConn(){
    if(pool_.empty()){
        std::cout << pool_.size() << std::endl;
        LOG_WARN("SqlConnPool Busy");
        return nullptr;
    }
    MYSQL* res;
    sem_wait(&sqlPoolSem);
    {
        std::lock_guard<std::mutex> locker(this->mtx_);
        res = pool_.front();
        pool_.pop();
    }
    return res;
}

void SqlConnPool::freeConn(MYSQL* conn){
    assert(conn != nullptr);
    std::lock_guard<std::mutex> locker(this->mtx_);
    pool_.push(conn);
    sem_post(&sqlPoolSem);
}

int SqlConnPool::getFreeCount() {
    std::lock_guard<std::mutex> locker(mtx_);
    return static_cast<int>(pool_.size());
}

void SqlConnPool::closePool() {
    std::lock_guard<std::mutex> locker(mtx_);
    while(!pool_.empty()){
        mysql_close(pool_.front());
        pool_.pop();
    }
    // 用于释放 MySQL相关资源，不调用可能会导致内存泄漏
    mysql_library_end();
}

SqlConnPool::~SqlConnPool() {
    closePool();
}