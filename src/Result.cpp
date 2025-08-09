#include "Result.hpp"

Result::Result(std::shared_ptr<Task> task, bool isValid)
    : m_taskPtr(task)
    , m_isValid(isValid)
{
    task->setResult(this);
}

void Result::setValue(Any any)
{   
    m_any = std::move(any);
    m_semaphore.post();
}

Any Result::get()
{
    if(!m_isValid)
        return "";
    m_semaphore.wait();
    return std::move(m_any);

}
