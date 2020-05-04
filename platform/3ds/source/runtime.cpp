#include "common/runtime.h"
#include "luasocket.h"
#include "common/assets.h"

u32 * SOCKET_BUFFER;
Result SOCKET_INITED;

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

    SOCKET_BUFFER = (u32 *)memalign(0x1000, SO_MAX_BUFSIZE);
    SOCKET_INITED = socInit(SOCKET_BUFFER, SO_MAX_BUFSIZE);
}

void userAppExit()
{
    if (Assets::IsFused())
        romfsExit();

    if (R_SUCCEEDED(SOCKET_INITED))
    {
        socExit();
        free(SOCKET_BUFFER);
    }

    acExit();
    ptmuExit();
    mcuHwcExit();
    cfguExit();
}
