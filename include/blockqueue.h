/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-03-30 10:26:53
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-01 14:07:20
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

    void push_back(const T& item);

    void push_front(const T& item);

    bool pop(T& item);

    bool pop(T& item, int timeout);

    /**
     * @brief 刷新缓冲区。
     *        通知消费者，队列中有数据可消费
     *        在此项目中就是同时日志系统，有日志需要记录
     * @return {*}
     */    
    bool flush();

private:
    std::deque<T> que;
    size_t capacity_;
    std::mutex mtx;
    bool isClose;
    std::condition_variable consumer;
    std::condition_variable producer;
    sem sem_;
};


#endif // BLOCK_QUEUE_H