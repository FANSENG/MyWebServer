#ifndef CONDITION_H
#define CONDITION_H

#include <condition_variable>
#include <shared_mutex>
#include <chrono>
using namespace std::chrono_literals;

/**
 * @brief condition 与读写锁配合使用
 * 当有线程需要 lock_shared / lock 读写锁，但当前读写锁不能被该线程使用
 * 该线程可以使用 condition 进行等待读写锁的释放
 * 占有读写锁的线程在释放时也可使用 notify_one 或者 notify_all 唤醒其他阻塞的线程
 */
class condition
{
public:
    /// @brief 初始化 condition 中的 条件变量 和 时间片
    condition();

    /// @brief 销毁 condition 中的 条件变量
    ~condition();

    /**
     * @brief 阻塞调用此函数的线程，等待 m_mutex 被释放
     * @param m_mutex 当前线程申请的锁
     */
    void wait(std::shared_mutex *m_mutex);

    /**
     * @brief 阻塞调用此函数的线程，等待 m_mutex 被释放，最多等待 time * 100ms
     * @param 最多等待的时间片，如果超过这个时间则取消阻塞
     * @return 返回 true 则表明获得了锁；返回 false 表示超时未获得锁
     */
    bool wait_for(std::shared_mutex *m_mutex, int time);

    /// @brief 唤醒一个阻塞的线程(具体唤醒那一个由操作系统决定)
    void notify_one();

    /// @brief 唤醒所有阻塞的线程
    void notify_all();

private:
    std::condition_variable_any *m_cond;
    static std::chrono::milliseconds time_slice;
};

#endif // CONDITION_H