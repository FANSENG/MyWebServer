/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}P
 * @Date: 2023-04-01 23:46:52
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-02 00:45:31
 */
#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <error.h>

class Epoller{
public:
    explicit Epoller(int maxEvent = 1024);

    ~Epoller();

    bool addFd(int fd, uint32_t event);

    bool modFd(int fd, uint32_t event);

    bool delFd(int fd);

    int wait(int timeoutMs = -1);

    int getEvnetFd(size_t index) const;

    uint32_t getEvnets(size_t index) const;


private:
    int epollfd;
    int maxEvent;
    std::vector<epoll_event> events;
};

#endif // EPOLLER_H