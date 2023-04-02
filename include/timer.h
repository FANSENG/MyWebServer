/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 19:10:42
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-01 23:40:33
 */
#ifndef TIMER_H
#define TIMER_H

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include <chrono>
#include "log.h"

typedef std::function<void()> TimeoutCallback;      // 回调函数
typedef std::chrono::high_resolution_clock Clock;   // 高精度时钟
typedef std::chrono::microseconds MS;               // 毫秒
typedef Clock::time_point TimeStamp;                // 时间戳

struct TimeNode{
    int id;             // 对应对象的唯一标识码，本项目中为client fd
    TimeStamp expires;  // 超时时间
    TimeoutCallback cb; // 回调函数

    bool operator<(const TimeNode& t){
        return expires < t.expires;
    };

    bool operator<=(const TimeNode &t){
        return expires <= t.expires;
    };

    bool operator>(const TimeNode& t){
        return expires > t.expires;
    };

    bool operator>=(const TimeNode &t){
        return expires >= t.expires;
    };
};

class HeapTimer{
public:
    HeapTimer(){ heap.reserve(64); }
    ~HeapTimer(){ clear(); }

    /**
     * @brief 为 head[id] 设置新的超时时间
     * @param {int} id
     * @param {int} newExpires
     * @return {*}
     */    
    void adjust(int id, int newExpires);

    /**
     * @brief 新增一个服务，设置该服务的过期时间
     * @param {int} id client 的标识码
     * @param {int} timeOut 超时时间
     * @param {TimeoutCallback&} cb 回调函数
     * @return {*}
     */    
    void add(int id, int timeOut, const TimeoutCallback& cb);
    
    /**
     * @brief 触发回调函数，并删除timerNode
     * @param {int} id
     * @return {*}
     */    
    void doWork(int id);

    /**
     * @brief 清空队列
     * @return {*}
     */    
    void clear();

    /**
     * @brief 清除超时定时器
     * @return {*}
     */    
    void tick();

    /**
     * @brief 弹出堆顶节点，即最小超时时间节点
     * @return {*}
     */    
    void pop();
    // void getNextTick();
private:
    /**
     * @brief 删除 堆&队列中的 index 节点
     * @param {size_t} item
     * @return {*}
     */    
    void del_(size_t index);

    /**
     * @brief 向上调整堆
     * @param {size_t} index
     * @return {*}
     */  
    void siftup_(size_t index);

    /**
     * @brief 向下调整堆
     * @param {size_t} index
     * @param {size_t} n
     * @return {*}
     */    
    bool siftdown_(size_t index, size_t n);

    /**
     * @brief 交换两个节点
     * @param {size_t} index1
     * @param {size_t} index2
     * @return {*}
     */    
    void swapNode_(size_t index1, size_t index2);

private:
    std::vector<TimeNode> heap;             // 计时器，每一个对应一个 Client
    std::unordered_map<int, size_t> ref;    // TimeNode.id -> index in heap
};

#endif // TIMER_H