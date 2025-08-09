#pragma once 

#include "Any.hpp"
// 
class Result;

class Task
{
public:
    Task() = default;
    ~Task() = default;
    virtual Any run() = 0;

    void setResult(Result* result);
    void execute();
private:
    // 任务返回结果
    Result* m_result;
};