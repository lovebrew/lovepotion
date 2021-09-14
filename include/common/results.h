#pragma once

#if defined(__SWITCH__)
    #include <switch.h>
    #define __CONSOLE_ABORT(res_expr) diagAbortWithResult(res_expr)
    #define MAX_GAMEPADS              4
#elif defined(__3DS__)
    #include <3ds.h>
    #define __CONSOLE_ABORT(res_expr) svcBreak(USERBREAK_PANIC)
#endif

#include <string>

static std::string LOVE_STRING_EMPTY;

/*
** Evaluates an expression that returns a result and
** returns the result if it would fail.
*/
#define R_TRY(res_expr)                        \
    ({                                         \
        const auto _tmp_r_try_rc = (res_expr); \
        if (R_FAILED(_tmp_r_try_rc))           \
            return _tmp_r_try_rc;              \
    })

/*
** Evaluates an expression that returns a result and
** returns the throw_result if it would fail.
*/
#define R_UNLESS(res_expr, throw_result)       \
    ({                                         \
        const auto _tmp_r_try_rc = (res_expr); \
        if (R_FAILED(_tmp_r_try_rc))           \
            return (throw_result);             \
    })

/*
** Evaluates an expression that returns a result and
** calls the apropriate abort method if it fails
** Only used during userAppInit()
*/
#define R_ABORT_UNLESS(res_expr)               \
    ({                                         \
        const auto _tmp_r_try_rc = (res_expr); \
        if (R_FAILED(_tmp_r_try_rc))           \
            __CONSOLE_ABORT(_tmp_r_try_rc);    \
    })

#define R_ABORT_LAMBDA_UNLESS(res_expr, lambda) \
    ({                                          \
        const auto _tmp_r_try_rc = (res_expr);  \
        const auto _tmp_lambda   = (lambda);    \
        if (R_FAILED(_tmp_r_try_rc))            \
        {                                       \
            _tmp_lambda();                      \
            __CONSOLE_ABORT(_tmp_r_try_rc);     \
        }                                       \
    })
