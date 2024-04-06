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

        /**
         * @brief Check if the result was successful (>= 0).
         */
        bool success() const
        {
            return this->value >= 0;
        }

        /**
         * @brief Check if the result was a failure (< 0).
         */
        bool failed() const
        {
            return this->value < 0;
        }

        /**
         * @brief Check if the result was a failure with a specific error code.
         */
        bool failed(int32_t error) const
        {
            return this->value == error;
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
