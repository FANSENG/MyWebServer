/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 19:11:31
 * @LastEditors: fs1n
 * @LastEditTime: 2023-03-30 20:01:48
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace webserver{

namespace Config{
    const std::string LOG_PATH = "./log";
    const std::string LOG_SUFFIX = ".log";
    const int LOG_MAXCAPACITY = 1024;   
}

}

#endif // CONFIG_H