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
#include <cassert>

// 其实这个线程池最好也用单例模式
// 只返回一个可操作ThreadPool的指针
class ThreadPool{
public:
    /// @brief explicit 表示禁止隐式转换类型，threadCount 类型必须为 size_t
    /// @param threadCount 
    explicit ThreadPool(size_t threadCount = 8): pool_(std::make_shared<Pool>()){
        assert(threadCount > 0);
        for(int i = 0; i < threadCount; i++){
            // 多线程同时处理 tasks 队列中的任务
            std::thread([pool = pool_]{
                std::unique_lock<std::mutex> locker(pool->mtx_);
                while(true){
                    // 有任务进入if分支
                    if(pool->isClose_) break; 
                    if(!pool->tasks.empty()){
                        // 从队列取出 task
                        // 这里使用了移动赋值，
                        auto task = std::move(pool->tasks.front());
                        pool->tasks.pop();

                        locker.unlock();
                        task();
                        locker.lock();
                    } 
                    else pool->cond.wait(locker);
                }
            }).detach();
        }
    }

    ThreadPool() = default;
    ThreadPool(ThreadPool&&) = default;
    ~ThreadPool(){
        if(pool_ != nullptr){
            {
                std::lock_guard<std::mutex> locker(pool_->mtx_);
                pool_->isClose_ = true;
            }
            pool_->cond.notify_all();
        }
    }

    /// @brief 增加任务
    /// @tparam F 
    /// @param task 任务，通过右值引用传递，允许传递右值
    template<class F>
    void addTask(F&& task){
        {
            std::lock_guard<std::mutex> locker(pool_->mtx_);
            // 完美转发，保证传递过程中右值引用始终为右值引用
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cond.notify_one();
    }

private:
    struct Pool{
        std::mutex mtx_;
        std::condition_variable cond;
        bool isClose_;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};

#endif // THREADPOOL_H