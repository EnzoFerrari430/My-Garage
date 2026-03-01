#pragma once

#include "Derived.h"

// 目标: impl里的parent_调用到 Base类中的PrintBase函数
struct Derived::Impl {
    Derived* parent_{nullptr};
};