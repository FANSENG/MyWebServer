/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 18:52:24
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-04 00:23:04
 */
#include <iostream>
#include "server/webserver.h"

int main(){
    WebServer server(
        8079, 3, 60000, false,              // prot, ET, timesout(ms), optLiner
        3306, "root", "root", "webserver", /* Mysql配置 */
        12, 6, true, LogLevel::INFO, 1024);             /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
    server.start();
    return 0;
}