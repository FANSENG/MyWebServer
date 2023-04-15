/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-02 10:15:49
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-05 16:45:36
 */

/**
 * ================================
 * 数据库连接池，统一管理数据库连接
 * ================================
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
    /// @brief 单例模式 获取 Sql 连接线程池指针
    /// @return 
    static SqlConnPool* Instance();

    /// @brief 获取一个 Sql 连接
    /// @return 
    MYSQL* getConn();

    /// @brief 释放连接
    /// @param conn 
    void freeConn(MYSQL* conn);

    /// @brief 获取可用连接数量
    /// @return 
    int getFreeCount();

    /// @brief 初始化连接池
    /// @param host 数据库地址
    /// @param port 数据库端口
    /// @param user 数据库用户名
    /// @param psw 数据库密码
    /// @param dbName 数据库名
    /// @param connSize 最大连接数
    void init(const char* host, int port,
              const char* user, const char* psw,
              const char* dbName, int connSize);
    
    /// @brief 关闭数据库连接
    void closePool();

private:
    SqlConnPool() = default;
    ~SqlConnPool();

    int MAXCONN;
    int userCount;

    std::queue<MYSQL*> pool_;
    std::mutex mtx_;
    sem_t sqlPoolSem;
};


#endif