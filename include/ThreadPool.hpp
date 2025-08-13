#pragma once 

#include "Thread.hpp"
#include "Task.hpp"
#include <mutex>
#include <string>
#include <vector>
#include <queue>
#include <condition_variable>
#include <memory>
#include <atomic>
#include "Result.hpp"
#include <unordered_map>


enum class PoolMode {
    MODE_FIXED,
    MODE_CACHED,
 };

const uint TASK_QUEUS_MAX_THRESHHOLD = 1000;
const uint THREAD_MAX_THRESHHOLD = 10;
const uint THREAD_MAX_IDLE_TIME = 60;


class ThreadPool
{
public:

    ThreadPool();
    ~ThreadPool();

    void setMode(const PoolMode& mode);
    void setThreadNumber(const uint& threadNumber);
    void setTaskQueMaxThreshHold(const uint& maxThreshHold);
    void setThreadMaxThreshHold(const uint& threadNum);

    Result submitTask(std::shared_ptr<Task> task);

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
    std::queue<std::shared_ptr<Task>> m_taskQueue;

    // 条件变量
    std::condition_variable m_notFull;
    std::condition_variable m_notEmpty;
    std::condition_variable m_exitCondition;

    // 设置线程池模式
    PoolMode m_mode;
    // 设置线程池的状态
    std::atomic_bool m_poolCondition;
};


