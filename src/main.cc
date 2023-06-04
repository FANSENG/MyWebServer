/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 18:52:24
 * @LastEditors: fs1n
 * @LastEditTime: 2023-06-03 14:07:15
 */
#include <iostream>
#include "log.h"
#include "webserver.h"

int main(){
    WebServer server(
        Config::SYS_PORT,
        Config::SYS_TRIGEMODE,
        Config::SYS_TIMEOUT,
        Config::SYS_OPTLINER,
        Config::SQL_PORT,
        Config::SQL_USERNAME.c_str(),
        Config::SQL_PSW.c_str(),
        Config::SQL_DBNAME.c_str(),
        Config::SQL_CONNNUM,
        Config::SYS_THREADNUM,
        Config::LOG_OPENLOG,
        // LogLevel::DEBUG,
        LogLevel::ERROR,
        Config::LOG_MAXCAPACITY
    );             /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
    server.start();
}