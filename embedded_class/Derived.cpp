#include "Derived.h"
#include "impl.h"

#include <iostream>

InputCallbackFunc g_callback{nullptr};
void* g_data{nullptr};

void Derived::ImplDeleter::operator()(Derived::Impl* p) const {
    delete p;
}

Derived::Derived(): pImpl_(new Impl(), ImplDeleter()){
    pImpl_->parent_ = this;
}

void Derived::Init() {

    g_callback = Derived::InputCallback;
    g_data = pImpl_.get();
}

void Derived::DoFunc() {

    char c;
    while(std::cin>> c) {
        std::cout << c << std::endl;
        g_callback(c, g_data);
    }

}

void Derived::InputCallback(char c, void *user_data) {

    if (c == 'a') {
        Derived::Impl* impl = static_cast<Derived::Impl*>(user_data);
        Derived* d = impl->parent_;
        std::cout << "InputCallback" << std::endl;
        d->PrintBase();
    }

}
