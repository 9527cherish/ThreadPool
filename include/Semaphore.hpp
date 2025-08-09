#pragma once 
#include <sys/types.h>
#include <mutex>
#include <condition_variable>

class Semaphore
{
public:

    Semaphore(uint resLimit = 0) : m_resLimit(resLimit){};
    ~Semaphore() = default;

    void wait(){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [&]()->bool{
            return m_resLimit > 0;
        });
        m_resLimit--;
    }

    void post()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_resLimit++;
        m_condition.notify_all();
    }

private:
    uint m_resLimit;
    std::mutex m_mutex;
    std::condition_variable m_condition;

};