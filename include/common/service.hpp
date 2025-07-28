#pragma once

#include "common/Result.hpp"

#include <stdint.h>

#define BIND(func, ...)                                                                                  \
    +[]() {                                                                                              \
        static auto x = [](auto... args) {                                                               \
            if constexpr (std::is_same_v<std::invoke_result_t<decltype(func), decltype(args)...>, void>) \
            {                                                                                            \
                func(args...);                                                                           \
                return ResultCode(0);                                                                    \
            }                                                                                            \
            else                                                                                         \
            {                                                                                            \
                return ResultCode(func(args...));                                                        \
            }                                                                                            \
        };                                                                                               \
        return x(__VA_ARGS__);                                                                           \
    }

namespace love
{
    struct Service
    {
        const char* name;
        ResultCode (*init)();
        void (*exit)();
    };
} // namespace love
