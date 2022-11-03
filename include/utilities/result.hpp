#include <concepts>
#include <stdint.h>

#include <utilities/abort.hpp>

namespace love
{
    struct FreeDeleter
    {
        void operator()(void* ptr)
        {
            free(ptr);
        }
    };

    inline bool g_EarlyExit = false;

    // clang-format off
    template<typename T>
    concept StructV = requires(T a)
    {
        { a.value } -> std::same_as<int32_t>;
    };
    // clang-format on

    class ResultCode
    {
      public:
        ResultCode(int32_t result);

        // clang-format off
        template<StructV T>
        // clang-format on
        ResultCode(T result)
        {
            this->result = result.mValue;
        }

        bool Success() const;

        bool Failed() const;

        operator int32_t()
        {
            return result;
        }

      private:
        int32_t result;
    };

#define R_UNLESS(res_expr, throw_result)                         \
    ({                                                           \
        const auto _tmp_r_try_rc = love::ResultCode((res_expr)); \
        if (_tmp_r_try_rc.Failed())                              \
            return (throw_result);                               \
    })
} // namespace love
