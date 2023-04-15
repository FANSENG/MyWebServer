/**
 * ========================================
 * 存放即将写入 log 文件的日志信息。
 * 阻塞队列中的信息来自于 日志对象的 buffer
 * ========================================
*/

#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <deque>
#include <condition_variable>
#include <mutex>
#include <cassert>
#include <sys/time.h>

template<class T>
class BlockQueue{
public:
    explicit BlockQueue(size_t MaxCapacity = 1024);
    
    ~BlockQueue();

    /**
     * @brief 清空队列
     * @return {*}
     */    
    void clear();

    /**
     * @brief
     * @return {*}
     */    
    bool empty();

    /**
     * @brief: 
     * @return {*}
     */    
    bool full();

    /**
     * @brief 关闭队列
     * @return {*}
     */    
    void close();

    size_t size();

    size_t capacity();

    T front();
    
    T back();

    /// @brief 队列后方插入item，本项目用于插入buff中的日志
    /// @param item 
    void push_back(const T& item);

    /// @brief 插入 item
    /// @param item 
    void push_front(const T& item);

    /// @brief 弹出对象存入item中
    /// @param item 
    /// @return 
    bool pop(T& item);

    /// @brief 弹出对象，存入 item 中，若队列为空则最多阻塞 timeout s
    /// @param item 
    /// @param timeout 
    /// @return 
    bool pop(T& item, int timeout);

    /**
     * @brief 刷新缓冲区。
     *        通知消费者，队列中有数据可消费
     * 在此项目中，异步写日志时会调用阻塞队列中的pop函数
     * 如果队列为空pop函数会阻塞等待队列中有数据唤醒，此函数作用为唤醒阻塞线程(写线程)
     */    
    void flush();

private:
    std::deque<T> que_;  // 队列
    size_t capacity_;   // 容量
    std::mutex mtx_;     // 互斥锁
    bool isClose_;       // 是否关闭
    std::condition_variable consumer;   // 消费者条件变量(pop为消费者)
    std::condition_variable producer;   // 生产者条件变量(push为生产者)
};

template <class T>
BlockQueue<T>::BlockQueue(size_t MaxCapacity): capacity_(MaxCapacity){
    assert(MaxCapacity > 0);
    isClose_ = false;
}

template <class T>
BlockQueue<T>::~BlockQueue(){
    this->close();
}

template <class T>
void BlockQueue<T>::close(){
    {
        std::lock_guard<std::mutex> locker(mtx_);
        que_.clear();
        isClose_ = true;
    }
    consumer.notify_all();
    producer.notify_all();
}

template <class T>
void BlockQueue<T>::flush(){
    consumer.notify_one();
}

template <class T>
void BlockQueue<T>::clear(){
    std::lock_guard<std::mutex> locker(mtx_);
    que_.clear();
}

template <class T>
T BlockQueue<T>::front(){
    std::lock_guard<std::mutex> locker(mtx_);
    return que_.front();
}

template <class T>
T BlockQueue<T>::back(){
    std::lock_guard<std::mutex> locker(mtx_);
    return que_.back();
}

template <class T>
size_t BlockQueue<T>::size(){
    std::lock_guard<std::mutex> locker(mtx_);
    return que_.size();
}

template <class T>
size_t BlockQueue<T>::capacity(){
    std::lock_guard<std::mutex> locker(mtx_);
    return capacity_;
}


template <class T>
void BlockQueue<T>::push_back(const T& item){
    // unique_lock 超出范围时也会自动解锁
    // 通常使用 lock_guard 的原因是更轻量
    // 在 C++ 17 中如果同时对多个 互斥量 上锁
    // 推荐使用 scope_lock，它会自动调整互斥量的上锁顺序
    // 防止因为上锁顺序而产生死锁
    std::unique_lock<std::mutex> locker(mtx_);
    while(que_.size() >= capacity_){
        producer.wait(locker);
    }
    que_.push_back(item);
    // sem_.post();
    consumer.notify_one();
}

template <class T>
void BlockQueue<T>::push_front(const T& item){
    std::unique_lock<std::mutex> locker(mtx_);
    while(que_.size() >= capacity_){
        producer.wait(locker);
    }
    que_.push_front(item);
    consumer.notify_one();
}

template <class T>
bool BlockQueue<T>::empty(){
    std::lock_guard<std::mutex> locker(mtx_);
    return que_.empty();
}

template <class T>
bool BlockQueue<T>::full(){
    std::lock_guard<std::mutex> locker(mtx_);
    return que_.size() >= capacity_;
}

template <class T>
bool BlockQueue<T>::pop(T& item){
    // std::lock_guard<mutex>(mtx);
    std::unique_lock<std::mutex> locker(mtx_);
    while(que_.empty()){
        consumer.wait(locker);
        if(isClose_) return false;
    }
    item = que_.front();
    que_.pop_front();
    producer.notify_one();
    return true;
}

template <class T>
bool BlockQueue<T>::pop(T& item, int timeout){
    std::unique_lock<std::mutex> locker(mtx_);
    while(que_.empty()){
        if(consumer.wait_for(locker, std::chrono::seconds(timeout))
            == std::cv_status::timeout || isClose_) return false;
    }
    item = que_.front();
    que_.pop_front();
    producer.notify_one();
    return true;
}

#endif // BLOCK_QUEUE_H