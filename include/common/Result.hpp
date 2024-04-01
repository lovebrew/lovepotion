#pragma once

#include <concepts>
#include <stdint.h>

namespace love
{
    // clang-format off
    template<typename T>
    concept NNResult = requires(T t)
    {
        { t.value } -> std::same_as<int32_t&>;
    };
    // clang-format on

    class Result
    {
      public:
        Result() : value(0)
        {}

        Result(int32_t value) : value(value)
        {}

        template<NNResult T>
        Result(T t) : value(t.value)
        {}

        bool success() const
        {
            return this->value >= 0;
        }

        bool failed() const
        {
            return this->value < 0;
        }

        int32_t get() const
        {
            return this->value;
        }

        explicit operator bool() const
        {
            return this->success();
        }

      private:
        int32_t value;
    };
} // namespace love
