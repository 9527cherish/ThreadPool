
#include "Thread.hpp"



uint Thread::generateId = 0;


Thread::Thread(ThreadFunc func)
    : m_func(func)
    , m_threadId(generateId++)
{

}

Thread::~Thread()
{
}

void Thread::start()
{
    std::thread t(m_func, m_threadId);
    t.detach();
}

uint Thread::threadId()
{
    return m_threadId;
}
