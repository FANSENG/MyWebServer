#ifndef LOCKER_H
#define LOCKER_H

#include <shared_mutex>

// ? 封装了毫无意义的一层，不如直接考虑使用 RAII 原理包装一下
class locker
{
public:
    locker();
    ~locker();
    void lock();
    bool try_lock();
    void unlock();
    void share_lock();
    bool try_share_lock();
    void share_unlock();
    std::shared_mutex *get();
private:
    std::shared_mutex* m_mutex;
};

// 一个简易的 locker_guard 实现
class locker_guard{
public:
    // 不允许 默认构造 & 赋值 & copy
    locker_guard() = delete;
    locker_guard(const locker_guard&) = delete;
    locker_guard& operator=(const locker_guard&) = delete;
    
    locker_guard(locker &lock){
        this->lock = &lock;
        this->lock->lock();
    }
    ~locker_guard(){
        if(this->lock != nullptr){
            this->lock->unlock();
        }
    }
    
private:
    locker* lock;
};

#endif // LOCKER_H