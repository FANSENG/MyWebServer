/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-03-30 10:26:53
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-04 14:12:49
 */

/**
 * ========================================
 * 存放即将写入 log 文件的日志信息。
 * 阻塞队列中的信息来自于 日志对象的 buffer
 * ========================================
*/

#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <deque>
#include <condition.h>
#include <mutex>
#include "sem.h"

template<class T>
class BlockQueue{
public:
    explicit BlockQueue(int MaxCapacity = 1024);
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
     * @return {*}
     */    
    bool flush();

private:
    std::deque<T> que;  // 队列
    size_t capacity_;   // 容量
    std::mutex mtx;     // 互斥锁
    bool isClose;       // 是否关闭
    std::condition_variable consumer;   // 消费者条件变量(pop为消费者)
    std::condition_variable producer;   // 生产者条件变量(push为生产者)
    sem sem_;                           // 信号量，可以代替条件变量
};


#endif // BLOCK_QUEUE_H