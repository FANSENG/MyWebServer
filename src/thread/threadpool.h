/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 19:10:32
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-04 16:55:40
 */

/**
 * ============================================
 * 线程池，管理线程资源，用内存空间换取时间的操作
 * threadCount 个线程在线程池创建时便存在
 * 在线程池销毁时线程销毁
 * ============================================
*/

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include <assert.h>

class ThreadPool{
public:
    /// @brief explicit 表示禁止隐式转换类型，threadCount 类型必须为 size_t
    /// @param threadCount 
    explicit ThreadPool(size_t threadCount = 8);
    ThreadPool() = default;
    ThreadPool(ThreadPool&&) = default;
    ~ThreadPool();

    /// @brief 增加任务
    /// @tparam F 
    /// @param task 任务，通过右值引用传递，允许传递右值
    template<class F>
    void addTask(F&& task);

private:
    struct Pool{
        std::mutex mtx;
        std::condition_variable cond;
        bool isClose;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool;
};


#endif // THREADPOOL_H