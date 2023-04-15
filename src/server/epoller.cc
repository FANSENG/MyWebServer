/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-02 00:33:39
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-15 15:19:39
 */
#include "epoller.h"

#include <cstdio>

Epoller::Epoller(int maxEvent):events(maxEvent), epollfd_(epoll_create(512)){
    assert(events.size() > 0 && epollfd_ >= 0);
}

Epoller::~Epoller(){
    close(epollfd_);
}

bool Epoller::addFd(int fd, uint32_t evnet){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = evnet;
    return 0 == epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
}

bool Epoller::modFd(int fd, uint32_t event){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = event;
    return 0 == epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
}

bool Epoller::delFd(int fd){
    if(fd < 0) return false;
    epoll_event ev = {0};
    return 0 == epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev);
}

int Epoller::wait(int timeoutMs){
    // epollfd_ : epoll 的文件描述符
    // &events[0] : 存放 events 的首地址
    // events.size() : 最多存放 evennt 数量
    // timeoutMs : 最大等待毫秒数
    return epoll_wait(epollfd_, &events[0], static_cast<int>(events.size()), timeoutMs);
}

int Epoller::getEvnetFd(size_t index) const{
    assert(index >= 0 && index < events.size());
    return events[index].data.fd;
}

uint32_t Epoller::getEvnets(size_t index) const{
    assert(index >= 0 && index < events.size());
    return events[index].events;
}