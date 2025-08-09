#include "ThreadPool.hpp"
#include <iostream>
#include <chrono>


ThreadPool::ThreadPool() 
    : m_threadNumber(4)
    , m_taskQueMaxThreshHold(TASK_QUEUS_MAX_THRESHHOLD)
    , m_mode(PoolMode::MODE_FIXED)
    , m_poolCondition(false)
    , m_idleThreadNumber(0)
    , m_threadMaxThreshHold(THREAD_MAX_THRESHHOLD)
    , m_CurrentThreadNumber(0)
{
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::setMode(const PoolMode &mode)
{
    if(!m_poolCondition)
        return;
    m_mode = mode;
}

void ThreadPool::setThreadNumber(const uint &threadNumber)
{
    m_threadNumber = threadNumber;
}

void ThreadPool::setTaskQueMaxThreshHold(const uint &maxThreshHold)
{
    m_taskQueMaxThreshHold = maxThreshHold;
}

void ThreadPool::setThreadMaxThreshHold(uint threadNum)
{
    if(PoolMode::MODE_CACHED != m_mode)
        return;

    m_threadMaxThreshHold = threadNum;
}

Result ThreadPool::submitTask(std::shared_ptr<Task> task)
{
    // 首先获取到锁
    std::unique_lock<std::mutex> lock(m_taskQueueMutex);
    // 再判断该任务队列有没有塞满，如果任务队列满了，任务提交失败
    // lambda表达式  返回 false → 条件不满足 → 线程阻塞
    // 返回 true → 条件满足 → 不阻塞，直接返回
    if(!m_notFull.wait_for(lock, std::chrono::seconds(1), [&]()->bool{
        return m_taskQueue.size() < TASK_QUEUS_MAX_THRESHHOLD;}))
        {
            std::cerr << "线程队列已满，任务提交失败" << std::endl;
            return Result(task, false);
        }

    // if(PoolMode::MODE_CACHED == m_mode
    //     && )

    // 如果没有满，任务提交成功，通知线程执行任务
    m_taskQueue.emplace(task);

    m_notEmpty.notify_all();
    return Result(task);
}

void ThreadPool::start()
{
    m_poolCondition = true;
    m_CurrentThreadNumber = m_threadNumber;

    for(uint i = 0; i < m_threadNumber; i++)
    {
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
        m_threads.emplace_back(std::move(ptr));
    }

    for(uint i = 0; i < m_threadNumber; i++)
    {
        m_threads[i]->start();
        m_idleThreadNumber++;
    }
}

// 线程任务执行函数
void ThreadPool::threadFunc()
{
    // 线程首先一直处于一个运行的状态，detach的线程在线程函数运行结束后会自动结束析构掉
    while(1)
    {
        std::shared_ptr<Task> task;
        {
            // 首先获取到锁，这个锁加锁的对象应该是任务队列
            std::unique_lock<std::mutex> lock(m_taskQueueMutex);
            
            std::cout << "线程id为:" << std::this_thread::get_id() << "的线程尝试获取任务" <<  std::endl;
            // 判断任务队列是否有任务，如果没有则等待
            m_notEmpty.wait(lock, [&]()->bool{
                return m_taskQueue.size() > 0;
            });

            std::cout << "线程id为:" << std::this_thread::get_id() << "获取到任务" <<  std::endl;
            m_idleThreadNumber--;
            // 如果有任务，则从任务队列中取出一个任务执行
            task = m_taskQueue.front();
            m_taskQueue.pop();

            // 取出后再判断该任务队列中是否还有任务，如果有，则唤醒其他正在等待的线程
            if(m_taskQueue.size() > 0)
            {
                m_notEmpty.notify_all();
            }
            m_notFull.notify_all();
        }
        // 拿到任务后，就将队列的锁释放
        if(nullptr != task)
        {
            task->execute();
        }

        // 这里要++，任务执行完了
        m_idleThreadNumber++;
    }
}
