#pragma once 

#include "Task.hpp"
#include <iostream>
#include <thread>

using ULong = unsigned long long;

class MyTask : public Task
{
public:
    MyTask() = default;
    MyTask(ULong begin, ULong end)
    : m_begin(begin)
    , m_end(end)
    {

    };
    ~MyTask() = default;
    virtual Any run() override{
        ULong sum = 0;
        for(ULong i = m_begin; i <= m_end; i++)
        {
            sum = sum + i;
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return sum;
    };

private:
    ULong m_begin;
    ULong m_end;
    
};