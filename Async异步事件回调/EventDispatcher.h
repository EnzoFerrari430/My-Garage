#pragma once

#include <functional>
#include <future>
#include <stdexcept>
#include <iostream>

namespace Core::Events {

template<typename Ret, typename... Args>
using EventCallback = std::function<Ret(Args...)>;

// 异步事件调度器
class AsyncEventDispatcher {
public:
    template<typename... Args>
    std::future<void> PostEvent(EventCallback<void, Args...> callback, Args&&... args) {
        // std::async异步执行 (默认launch::async保证创建新线程)
        return std::async(std::launch::async, 
            [callback = std::move(callback), ...args = std::forward<Args>(args)] () mutable {
                try {
                    callback(std::forward<Args>(args)...);
                } catch (const std::exception& e) {
                    std::cerr << "事件回调执行异常: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "事件回调执行未知异常" << std::endl;
                }
            });
    }

    template<typename Ret, typename... Args>
    std::future<Ret> PostEventWithResult(EventCallback<Ret, Args...> callback, Args&&... args) {
        return std::async(std::launch::async,
            [callback = std::move(callback), ...args = std::forward<Args>(args)]() mutable -> Ret {
                try {
                    return callback(std::forward<Args>(args)...);
                } catch (const std::exception& e) {
                    std::cerr << "事件回调执行异常: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "事件回调执行未知异常" << std::endl;
                }
            });
    }

    // 等待所有异步事件完成
    template<typename... Futures>
    void WaitAll(Futures&&... futures) {
        (futures.wait(), ...); // C++17折叠表达式
    }

};

}