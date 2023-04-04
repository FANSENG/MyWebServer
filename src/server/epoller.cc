/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-02 00:33:39
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-04 15:12:31
 */
#include "epoller.h"

Epoller::Epoller(int maxEvnet): maxEvent(maxEvent), events(maxEvent), epollfd(epoll_create(1)){
    assert(maxEvent > 0 && epollfd >= 0);
}

Epoller::~Epoller(){
    close(epollfd);
}

bool Epoller::addFd(int fd, uint32_t evnet){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = evnet;
    return 0 == epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

bool Epoller::modFd(int fd, uint32_t event){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = event;
    return 0 == epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

bool Epoller::delFd(int fd){
    if(fd < 0) return false;
    epoll_event ev = {0};
    return 0 == epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

int Epoller::wait(int timeoutMs){
    /// epoll 的文件描述符
    /// 需要监听的 文件描述符
    /// 最大监听事件
    return epoll_wait(epollfd, &events[0], maxEvent, timeoutMs);
}

int Epoller::getEvnetFd(size_t index) const{
    assert(index >= 0 && index < maxEvent);
    return events[index].data.fd;
}

uint32_t Epoller::getEvnets(size_t index) const{
    assert(index >= 0 && index < maxEvent);
    return events[index].events;
}