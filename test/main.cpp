
#include "ThreadPool.hpp"
#include <chrono>
#include "MyTask.hpp"
#include "Result.hpp"


int main() {
    ThreadPool threadPool;
    threadPool.start();
    Result sum1 =  threadPool.submitTask(std::make_shared<MyTask>(1, 100000000));
    Result sum2 = threadPool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
    Result sum3 = threadPool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
    // Result sum4 = threadPool.submitTask(std::make_shared<MyTask>(300000001, 400000000));
    // threadPool.submitTask(std::make_shared<MyTask>(400000000, 500000000));
    // threadPool.submitTask(std::make_shared<MyTask>(500000000, 600000000));
    // threadPool.submitTask(std::make_shared<MyTask>(600000000, 700000000));
    // threadPool.submitTask(std::make_shared<MyTask>(700000000, 800000000));
    ULong sum = sum1.get().cast_<ULong>() + sum2.get().cast_<ULong>()
                    + sum3.get().cast_<ULong>(); // + sum4.get().cast_<ULong>();

    std::cout << "sum=" << sum << std::endl;

    std::getchar();
    return 0;
}