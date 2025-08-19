
#include "ThreadPool.hpp"
#include <chrono>

int add(int a, int b)
{
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return a + b;
}

float add(float a, float b, float c)
{
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return a + b + c;
}

int main() {
    {
        ThreadPool threadPool;
        // threadPool.setMode(PoolMode::MODE_CACHED);
        threadPool.start();
        std::future<int> sum1 =  threadPool.submitTask(
                static_cast<int(*)(int, int)>(add),  // 显式指定函数指针类型
                10, 20
            );
        std::future<int> sum2 =  threadPool.submitTask(
                static_cast<int(*)(int, int)>(add),  // 显式指定函数指针类型
                10, 20
            );
        std::future<int> sum3 =  threadPool.submitTask(
                static_cast<int(*)(int, int)>(add),  // 显式指定函数指针类型
                10, 20
            );
        std::future<float> sum4 =  threadPool.submitTask(
                static_cast<float(*)(float, float, float)>(add),  // 显式指定函数指针类型
                10.0, 20.0, 30.0
            );

        std::future<int> sum5 =  threadPool.submitTask(
                static_cast<int(*)(int, int)>(add),  // 显式指定函数指针类型
                10, 20
            );
        std::cout << "sum1=" << sum1.get() << std::endl;
        std::cout << "sum2=" << sum2.get() << std::endl;
        std::cout << "sum3=" << sum3.get() << std::endl;


        std::cout << "sum4=" << sum4.get() << std::endl;
        std::cout << "sum5=" << sum5.get() << std::endl;
    }
    std::getchar();
    return 0;
}