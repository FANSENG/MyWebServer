/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-03-30 10:27:05
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-01 14:08:02
 */
#include "blockqueue.h"

template <class T>
BlockQueue<T>::BlockQueue(int MaxCapacity = 1024) : capacity(MaxCapacity), {
    assert(MaxCapacity > 0)
    isClose = false;
}

template <class T>
BlockQueue<T>::~BlockQueue(){
    close();
}

template <class T>
void BlockQueue<T>::clear(){
    std::lock_guard<std::mutex>(mtx);
    que.clear();
    sem_.set(0);
}

template <class T>
bool BlockQueue<T>::empty(){
    std::lock_guard<mutex>(mtx);
    return que.empty();
}

template <class T>
bool BlockQueue<T>::full(){
    std::lock_guard<mutex> mtx;
    return que.size() >= capacity;
}

template <class T>
void BlockQueue<T>::close(){
    {
        std::lock_guard<mutex>(mtx);
        que.clear();
        isClose = true;
    }
    sem_.set(0);
    consumer.notify_all();
    producer.notify_all();
}

template <class T>
size_t BlockQueue<T>::size(){
    std::lock_guard<mutex>(mtx);
    return que.size();
}

template <class T>
size_t BlockQueue<T>::capacity(){
    std::lock_guard<mutex>(mtx);
    return capacity_;
}

template <class T>
T BlockQueue<T>::front(){
    std::lock_guard<mutex>(mtx);
    return que.front();
}

template <class T>
T BlockQueue<T>::back(){
    std::lock_guard<mutex>(mtx);
    return que.back();
}

template <class T>
void BlockQueue<T>::push_back(const T& item){
    // unique_lock 超出范围时也会自动解锁
    // 通常使用 lock_guard 的原因是更轻量
    // 在 C++ 17 中如果同时对多个 互斥量 上锁
    // 推荐使用 scope_lock，它会自动调整互斥量的上锁顺序
    // 防止因为上锁顺序而产生死锁
    std::unique_lock<mutex> locker(mtx);
    while(que.size >= capacity_){
        producer.wait(locker);
        
        // 也可以用下面三句替代上面
        // locker.unlock();
        // sem_.wait();
        // locker.lock();
    }
    que.push_back(item);
    sem_.post();
    consumer.notify_one();
}

template <class T>
void BlockQueue<T>::push_front(const T& item){
    std::unique_lock<mutex> locker(mutex);
    while(que.size() >= capacity_){
        producer.wait(locker);
    }
    que.push_front(item);
    consumer.notify_one();
}

template <class T>
bool BlockQueue<T>::pop(T& item){
    // std::lock_guard<mutex>(mtx);
    std::unique_lock<mutex> locker(mtx);
    while(que.empty()){
        consumer.wait(locker);
        if(isClose) return false;
    }
    item = que.front();
    que.pop_front();
    producer.notify_one();
    return true;
}

template <class T>
bool BlockQueue<T>::pop(T& item, int timeout){
    std::unique_lock<mutex> locker(mtx);
    while(que.empty()){
        if(consumer.wait_for(locker, std::chrono::seconds(timeout))
            == std::cv_status::timeout) return false;
        if(isClose) return false;
    }
    item = que.front();
    que.pop_front();
    producer.notify_one();
    return true;
}

template <class T>
bool BlockQueue<T>::flush(){
    consumer.notify_one();
}