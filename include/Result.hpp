#pragma once 

#include "Any.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"
#include <atomic>

class Result
{
public:
    Result(std::shared_ptr<Task> task, bool isValid = true);
    ~Result() = default;

    // 设置返回值
    void setValue(Any any);
    // 从Result中取到any
    Any get();
    
private:
    Any m_any;  // 用于接收返回值
    Semaphore m_semaphore;  // 用于判断线程是否执行完毕，执行完毕才可接收返回值
    std::shared_ptr<Task> m_taskPtr;  // 指向返回值对应的任务
    std::atomic_bool m_isValid;       // 判断该任务是否有效

};