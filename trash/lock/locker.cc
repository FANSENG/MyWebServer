#include "locker.h"

locker::locker(){
    m_mutex = new std::shared_mutex();
}

locker::~locker(){
    delete m_mutex;
}

void locker::lock(){
    m_mutex->lock();
}

bool locker::try_lock(){
    return m_mutex->try_lock();
}

void locker::unlock(){
    m_mutex->unlock();
}

void locker::share_lock(){
    m_mutex->lock_shared();
}

bool locker::try_share_lock(){
    return m_mutex->try_lock_shared();
}

void locker::share_unlock(){
    m_mutex->lock_shared();
}

std::shared_mutex* locker::get(){
    return m_mutex;
}