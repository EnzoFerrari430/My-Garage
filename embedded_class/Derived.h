#pragma once

#include "Base.h"

#include <memory>

typedef void(*InputCallbackFunc)(char, void*);

class Derived : public Base {
public:
    Derived();

    void Init();
    void DoFunc();

    static void InputCallback(char c, void* user_data);

private:
    struct Impl;
    struct ImplDeleter {
        void operator()(Impl* p) const;
    };
    std::unique_ptr<Impl, ImplDeleter> pImpl_;
};
