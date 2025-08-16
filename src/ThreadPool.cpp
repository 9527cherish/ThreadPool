#include "ThreadPool.hpp"
#include <iostream>
#include <chrono>


ThreadPool::ThreadPool() 
    : m_threadNumber(4)
    , m_CurrentThreadNumber(0)
    , m_idleThreadNumber(0)
    , m_threadMaxThreshHold(THREAD_MAX_THRESHHOLD)
    , m_taskSize(0)
    , m_taskQueMaxThreshHold(TASK_QUEUS_MAX_THRESHHOLD)
    , m_mode(PoolMode::MODE_FIXED)
    , m_poolCondition(false)
{
}

ThreadPool::~ThreadPool()
{
    m_poolCondition = false;
    m_notEmpty.notify_all();
    std::unique_lock<std::mutex> lock(m_taskQueueMutex);
    // std::this_thread::sleep_for(std::chrono::seconds(3));
    m_exitCondition.wait(lock, [&]()->bool{return m_threads.size() == 0;});
}

void ThreadPool::setMode(const PoolMode &mode)
{
    if(m_poolCondition)
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

void ThreadPool::setThreadMaxThreshHold(const uint &threadNum)
{
    if(PoolMode::MODE_CACHED != m_mode)
        return;

    m_threadMaxThreshHold = threadNum;
}

void ThreadPool::start()
{
    m_poolCondition = true;
    m_CurrentThreadNumber = m_threadNumber;

    for(uint i = 0; i < m_threadNumber; i++)
    {
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        uint threadId = ptr->threadId();
        m_threads.emplace(threadId, std::move(ptr));
    }

    for(uint i = 0; i < m_threadNumber; i++)
    {
        m_threads[i]->start();
        m_idleThreadNumber++;
    }
}

// 线程任务执行函数
void ThreadPool::threadFunc(uint threadId)
{
    auto lastTime = std::chrono::high_resolution_clock().now();

    // 线程首先一直处于一个运行的状态，detach的线程在线程函数运行结束后会自动结束析构掉
    while(m_poolCondition)
    {
        Task task;
        {
            // 首先获取到锁，这个锁加锁的对象应该是任务队列
            std::unique_lock<std::mutex> lock(m_taskQueueMutex);
            
            std::cout << "线程id为:" << std::this_thread::get_id() << "的线程尝试获取任务" <<  std::endl;

            while(m_taskQueue.size() == 0)
            {
                if(m_mode == PoolMode::MODE_CACHED)
                {
                    if (std::cv_status::timeout == m_notEmpty.wait_for(lock, std::chrono::seconds(1)))
                    {
                        auto curTime = std::chrono::high_resolution_clock().now();
                        auto durTime = std::chrono::duration_cast<std::chrono::seconds>(curTime - lastTime);
                        if(durTime.count() > THREAD_MAX_IDLE_TIME
                            && (uint)m_CurrentThreadNumber > m_threadNumber)
                            {
                                m_threads.erase(threadId);
                                m_CurrentThreadNumber--;
                                m_idleThreadNumber--    ;
                                std::cout << "线程id为:" << std::this_thread::get_id() << "动态销毁" <<  std::endl;
                                return;
                            }
                    }
                }
                else
                {
                    m_notEmpty.wait(lock);
                }

                if(!m_poolCondition)
                {
                    std::cout << "线程id为:" << std::this_thread::get_id() << "动态销毁" <<  std::endl;
                    std::cout << "m_threads.size"  << m_threads.size() << std::endl;
                    m_threads.erase(threadId);
                    m_exitCondition.notify_all();
                    return;
                }

            }
            std::cout << "线程id为:" << std::this_thread::get_id() << "获取到任务" <<  std::endl;
            m_idleThreadNumber--;
            // 如果有任务，则从任务队列中取出一个任务执行
            task = m_taskQueue.front();
            m_taskQueue.pop();
            m_taskSize--;
            m_idleThreadNumber--;

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
            task();
        }

        // 这里要++，任务执行完了
        m_idleThreadNumber++;

        lastTime = std::chrono::high_resolution_clock().now();
    }

    m_threads.erase(threadId);
    
    std::cout << "线程id为:" << std::this_thread::get_id() << "动态销毁" <<  std::endl;
    std::cout << "m_threads.size"  << m_threads.size() << std::endl;
    m_exitCondition.notify_all();
    return;
}
