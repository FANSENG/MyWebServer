/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 19:11:31
 * @LastEditors: fs1n
 * @LastEditTime: 2023-05-31 18:10:10
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config{
    // LOG
    const std::string LOG_PATH = "./log";
    const std::string LOG_SUFFIX = ".log";
    const int LOG_MAXCAPACITY = 1024;
    const bool LOG_OPENLOG = true;

    // SQL
    const std::string SQL_IP = "localhost";
    const int SQL_PORT = 3306;
    const std::string SQL_USERNAME = "root";
    const std::string SQL_PSW = "";
    const std::string SQL_DBNAME = "webserver";
    const int SQL_CONNNUM = 12;

    // BASE
    const std::string SOURCE_DIR = "/home/ubuntu/project/cppproject/MyWebServer/resources";
    const int SYS_PORT = 8079;
    const int SYS_THREADNUM = 6;
    const int SYS_TIMEOUT = 60000;
    const int SYS_TRIGEMODE = 3;
    const bool SYS_OPTLINER = false;
    
}

#endif // CONFIG_H