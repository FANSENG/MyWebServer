/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}P
 * @Date: 2023-04-01 23:46:52
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-04 15:13:52
 */
/**
 * ===================================================
 * 对linux提供的IO事件通知机制进行包装, 对文件描述符管理
 * epoll 适用于高并发场景，其时间复杂度为O(1)
 * ===================================================
*/
#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <vector>
#include <error.h>

class Epoller{
public:
    /// @brief 禁止隐式类型转换
    /// @param maxEvent 
    explicit Epoller(int maxEvent = 1024);
    
    ~Epoller();

    /// @brief 添加监听的文件描述符 fd， event为监听的事件
    /// @param fd 
    /// @param event 
    /// @return 
    bool addFd(int fd, uint32_t event);

    /// @brief 修改文件描述符fd 的监听事件
    /// @param fd 
    /// @param event 
    /// @return 
    bool modFd(int fd, uint32_t event);

    /// @brief 删除对 文件描述符fd 的监听
    /// @param fd 
    /// @return 
    bool delFd(int fd);

    /// @brief 阻塞等待 epoll 事件 从 epoll 实例中发生
    /// @param timeoutMs 
    /// @return 返回准备好IO的文件描述符数量，返回0表示超时
    int wait(int timeoutMs = -1);

    /// @brief 获取 events 中下标为 index的event 对应的文件描述符
    /// @param index 
    /// @return 
    int getEvnetFd(size_t index) const;

    /// @brief 获取 events 中下标为 index的event 对应的监听事件
    /// @param index 
    /// @return 
    uint32_t getEvnets(size_t index) const;


private:
    int epollfd;
    int maxEvent;
    std::vector<epoll_event> events;
};

#endif // EPOLLER_H