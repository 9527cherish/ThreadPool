#pragma once 

#include <thread>
#include <functional>

using ThreadFunc = std::function<void(uint)>;

class Thread
{
public:
    Thread(ThreadFunc func);
    ~Thread();

    void start();
    static uint generateId;
    uint threadId();

private:
    ThreadFunc m_func;
    uint m_threadId;
};