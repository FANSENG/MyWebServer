/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-02 09:31:17
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-04 20:01:08
 */
#include "threadpool.h"

ThreadPool::ThreadPool(size_t threadCount): pool(std::make_shared<Pool>()){
    assert(threadCount > 0);
    auto f = [=]{
        std::unique_lock<std::mutex> locker(pool->mtx);
        while(true){
            // 有任务进入if分支
            if(pool->isClose) break; 
            else if(!pool->tasks.empty()){
                // 从队列取出 task
                auto task = move(pool->tasks.front());
                pool->tasks.pop();

                locker.unlock();
                task();
                locker.lock();
            } 
            else pool->cond.wait(locker);
        }

    };
    for(int i = 0; i < threadCount; i++){
        // 多线程同时处理 tasks 队列中的任务
        std::thread(f).detach();
    }

}

ThreadPool::~ThreadPool(){
    if(pool != nullptr){
        {
            std::lock_guard<std::mutex>(pool->mtx);
            pool->isClose = true;
        }
        pool->cond.notify_all();
    }
}

template<class F>
void ThreadPool::addTask(F&& task){
    {
        std::lock_guard<std::mutex>(pool->mtx);
        // 完美转发，保证传递过程中右值引用始终为右值引用
        pool->tasks.emplace(std::forward<F>(task));
    }
    pool->cond.notify_one();
}