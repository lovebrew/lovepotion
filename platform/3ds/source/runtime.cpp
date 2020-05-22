#include <3ds.h>

u32 * SOCKET_BUFFER;
Result SOCKET_INITED;

#define SO_MAX_BUFSIZE 0x100000
#define SO_BUF_ALIGN 0x1000

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

    SOCKET_BUFFER = (u32 *)memalign(SO_BUF_ALIGN, SO_MAX_BUFSIZE);
    SOCKET_INITED = socInit(SOCKET_BUFFER, SO_MAX_BUFSIZE);
}

void userAppExit()
{
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
