#pragma once 

#include "Thread.hpp"
#include <mutex>
#include <string>
#include <vector>
#include <queue>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <unordered_map>
#include <future>
#include <iostream>
#include <functional>


enum class PoolMode {
    MODE_FIXED,
    MODE_CACHED,
 };

const uint TASK_QUEUS_MAX_THRESHHOLD = 1000;
const uint THREAD_MAX_THRESHHOLD = 10;
const uint THREAD_MAX_IDLE_TIME = 60;


using Task = std::function<void()>;

class ThreadPool
{
public:

    ThreadPool();
    ~ThreadPool();

    void setMode(const PoolMode& mode);
    void setThreadNumber(const uint& threadNumber);
    void setTaskQueMaxThreshHold(const uint& maxThreshHold);
    void setThreadMaxThreshHold(const uint& threadNum);

    // 使用可变参模板变成，使其可以接收任意任意函数和任意数量的参数
    // pool.submitTask(func, a, b)

    // 
    template<typename Func, typename... Args>
    auto submitTask(Func&& func, Args&&... args)->std::future<decltype(func(args...))>
    {
        using Rtype = decltype(func(args...));
        auto task = std::make_shared<std::packaged_task<Rtype()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::future<Rtype> result = task->get_future();

        m_taskSize++;
        // 首先获取到锁
        std::unique_lock<std::mutex> lock(m_taskQueueMutex);
        if(PoolMode::MODE_CACHED == m_mode
            && m_taskSize > m_idleThreadNumber
            && (uint)m_CurrentThreadNumber <= m_threadMaxThreshHold)
        {
            // std::cout << m_taskSize << ", " <<  m_idleThreadNumber << std::endl;
            auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
            uint threadId = ptr->threadId();
            m_threads.emplace(threadId, std::move(ptr));
            m_threads[threadId]->start();
            m_CurrentThreadNumber++;
            m_idleThreadNumber++;
            std::cout << "动态创建线程" <<  std::endl;
        }   

        // 再判断该任务队列有没有塞满，如果任务队列满了，任务提交失败
        // lambda表达式  返回 false → 条件不满足 → 线程阻塞
        // 返回 true → 条件满足 → 不阻塞，直接返回
        if(!m_notFull.wait_for(lock, std::chrono::seconds(1), [&]()->bool{
            return m_taskQueue.size() < TASK_QUEUS_MAX_THRESHHOLD;}))
            {
                std::cerr << "线程队列已满，任务提交失败" << std::endl;
                auto task = std::make_shared<std::packaged_task<Rtype()>>(
                    []()->Rtype{return Rtype();});
                
                /**
                 * std::packaged_task 重载了函数调用运算符 operator()，因此可以像调用函数一样调用它。
                 * (*task)() 表示执行 packaged_task 中包装的任务（即通过 std::bind 绑定的 func 及其参数）。
                 */

                (*task)();
                return task->get_future();
            }

        // 如果没有满，任务提交成功，通知线程执行任务
        // 仅将 “执行 task 的逻辑” 放入队列，不会立即执行 task 函数，
        // 任务的实际执行发生在队列中的 lambda 被调用时（通常由线程池等调度逻辑触发）。
        m_taskQueue.emplace([task](){(*task)();});

        m_notEmpty.notify_all();
        return result;
    }

    void start();

    // 禁用左值拷贝构造和赋值
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    // 禁用右值拷贝构造和赋值
    ThreadPool(const ThreadPool&&) = delete;
    ThreadPool&& operator=(const ThreadPool&&) = delete;

private:
    // 线程函数，供线程使用
    void threadFunc(uint threadId);

private:
    // 线程数量
    uint m_threadNumber;
    // 正在运行线程数量
    std::atomic_int m_CurrentThreadNumber;
    // 空闲线程数量
    std::atomic_int m_idleThreadNumber;
    // 线程队列
    std::unordered_map<uint, std::unique_ptr<Thread>> m_threads;
    // 线程队列上限阈值
    uint m_threadMaxThreshHold;

    // 任务数量
    std::atomic_int m_taskSize;
    // 最大任务数
    uint m_taskQueMaxThreshHold;
    // 锁
    std::mutex m_taskQueueMutex;
    // 任务队列
    std::queue<Task> m_taskQueue;

    // 条件变量
    std::condition_variable m_notFull;
    std::condition_variable m_notEmpty;
    std::condition_variable m_exitCondition;

    // 设置线程池模式
    PoolMode m_mode;
    // 设置线程池的状态
    std::atomic_bool m_poolCondition;
};


