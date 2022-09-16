#include <concepts>
#include <stdint.h>

namespace love
{
    class ResultCode
    {
      public:
        ResultCode(int32_t result);

        // clang-format off
        template<typename T>
        ResultCode(T result)
        {
            this->result = result.mValue;
        }
        // clang-format on

        bool Success() const;

        bool Failed() const;

      private:
        int32_t result;
    };

#define R_UNLESS(res_expr, throw_result)                   \
    ({                                                     \
        const auto _tmp_r_try_rc = ResultCode((res_expr)); \
        if (_tmp_r_try_rc.Failed())                        \
            return (throw_result);                         \
    })
} // namespace love
