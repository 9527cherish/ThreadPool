#include "Task.hpp"
#include "Result.hpp"

void Task::setResult(Result* result)
{
    m_result = result;
}

void Task::execute()
{
    if(nullptr != m_result)
    {
        m_result->setValue(run());
    }
}
