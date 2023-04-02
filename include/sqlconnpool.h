/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-02 10:15:49
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-02 10:21:31
 */
#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "log.h"

class SqlConnPool{
public:
    static SqlConnPool* Instance();
    MYSQL* getConn();
    void freeConn(MYSQL* conn);
    int getFreeCount();
    int getUserCount();
    void init(const char* host, int port,
              const char* user, const char* psw,
              const char* dbName, int connSize);
    void closePool();

private:
    SqlConnPool();
    ~SqlConnPool();

    int MAXCONN;
    int userCount;
    int freeCount;

    std::queue<MYSQL*> pool;
    std::mutex mtx;
    sem_t sqlPoolSem;
};


#endif