#pragma once

#include <concepts>
#include <stdint.h>

namespace love
{
    // clang-format off
    template<typename T>
    concept NNResult = requires(T t)
    {
        { t.value } -> std::convertible_to<int32_t>;
    };
    // clang-format on

    class Result
    {
      public:
        Result(int32_t value) : value(value)
        {}

        template<NNResult T>
        Result(T t) : value(t.value)
        {}

        int32_t get() const
        {
            return this->value;
        }

        operator int32_t() const
        {
            return this->value;
        }

        bool success() const
        {
            return this->value >= 0;
        }

        bool failed() const
        {
            return this->value < 0;
        }

        operator bool() const
        {
            return this->success();
        }

        bool operator==(Result& other) const
        {
            return this->value == other.value;
        }

        bool operator==(int32_t value) const
        {
            return this->value == value;
        }

      private:
        int32_t value;
    };
} // namespace love
