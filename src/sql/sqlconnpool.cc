/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-02 10:15:32
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-02 14:41:42
 */

#include "sqlconnpool.h"

SqlConnPool* SqlConnPool::Instance(){
    static SqlConnPool sqlpool;
    return &sqlpool;
}

SqlConnPool::SqlConnPool(): userCount(0), freeCount(0){}

MYSQL* SqlConnPool::getConn(){
    if(pool.empty()){
        LOG_WARN("SqlConnPool Busy");
        return nullptr;
    }
    MYSQL* res;
    sem_wait(&sqlPoolSem);
    {
        std::lock_guard<std::mutex>(this->mtx);
        res = pool.front();
        pool.pop();
    }
    return res;
}