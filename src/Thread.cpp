
#include "Thread.hpp"


Thread::Thread(ThreadFunc func)
    : m_func(func)
{

}

Thread::~Thread()
{
}

void Thread::start()
{
    std::thread t(m_func);
    t.detach();
}
