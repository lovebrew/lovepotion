#include "common/runtime.h"

void userAppInit()
{
    Result res = cfguInit();

    if (R_FAILED(res))
        svcBreak(USERBREAK_PANIC);

    res = mcuHwcInit();

    if (R_FAILED(res))
        svcBreak(USERBREAK_PANIC);

    res = ptmuInit();

    if (R_FAILED(res))
        svcBreak(USERBREAK_PANIC);

    res = acInit();

    if (R_FAILED(res))
        svcBreak(USERBREAK_PANIC);
}

void userAppExit()
{
    acExit();
    ptmuExit();
    mcuHwcExit();
    cfguExit();
}
