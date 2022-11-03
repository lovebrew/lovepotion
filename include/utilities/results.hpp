#pragma once

#if defined(__SWITCH__)
    #include <switch.h>
    #define __CONSOLE_ABORT(res_expr) diagAbortWithResult(res_expr)
#elif defined(__3DS__)
    #include <3ds.h>
    #define __CONSOLE_ABORT(res_expr) svcBreak(USERBREAK_PANIC)
#endif

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
