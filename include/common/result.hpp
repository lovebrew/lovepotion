#pragma once

namespace love
{
#define R_UNLESS(res_expr, throw_result)       \
    ({                                         \
        const auto _tmp_r_try_rc = (res_expr); \
        if (res_expr < 0)                      \
            return (throw_result);             \
    }) // namespace love
} // namespace love
