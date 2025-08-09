#pragma once 

#include <thread>
#include <functional>

using ThreadFunc = std::function<void()>;

class Thread
{
public:
    Thread(ThreadFunc func);
    ~Thread();

    void start();

private:
    ThreadFunc m_func;
};