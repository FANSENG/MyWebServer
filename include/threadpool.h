/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 19:10:32
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-02 10:05:02
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
    explicit ThreadPool(size_t threadCount = 8);
    ThreadPool() = default;
    ThreadPool(ThreadPool&&) = default;
    ~ThreadPool();

    template<class F>
    void AddTask(F&& task);

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