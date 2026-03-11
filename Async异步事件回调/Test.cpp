#include "EventDispatcher.h"

#include <thread>
#include <string>
#include <chrono>

// 业务
void OnTaskCompleted(const std::string& taskName, int payload) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::cout << "[回调执行] 任务完成: " << taskName << " (ID: " << payload 
            << "), 执行线程ID: " << std::this_thread::get_id() << std::endl;
}

int CalculateSum(const std::string& taskName, int a, int b) {
    std::cout << "[计算中] 线程ID: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟耗时计算
    return a + b;
}

int main() {

    std::cout << "主线程ID: " << std::this_thread::get_id() << std::endl;

    Core::Events::AsyncEventDispatcher dispatcher;

    std::future<void> future1 = dispatcher.PostEvent<const std::string&, int>(OnTaskCompleted, "加载数据", 42);

    std::future<int> future2 = dispatcher.PostEventWithResult<int, const std::string&, int, int>(CalculateSum, "求和", 21, 32);

    // lambda表达式
    std::future<void> future3 = dispatcher.PostEvent<const std::string&, double>(
        [](const std::string& taskName, double progress){
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout << "[任务" << taskName << " 执行] : " << progress * 100 
            << "%, 执行线程ID: " << std::this_thread::get_id() << std::endl;
    }, "下载", 0.85);

    // dispatcher.WaitAll(future1, future2, future3);

    // 获取future2的值
    std::cout << "获取future2的值: " << future2.get() << std::endl;

    return 0;
}