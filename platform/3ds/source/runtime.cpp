#include <3ds.h>
#include <malloc.h>

u32 * SOCKET_BUFFER;
Result SOCKET_INITED;

#define SO_MAX_BUFSIZE 0x100000
#define SO_BUF_ALIGN 0x1000

extern "C"
{
    void userAppInit()
    {
        osSetSpeedupEnable(true);

        /* config info */
        Result res = cfguInit();

        if (R_FAILED(res))
            svcBreak(USERBREAK_PANIC);

        /* mcu:hwc for raw battery info */
        res = mcuHwcInit();

        if (R_FAILED(res))
            svcBreak(USERBREAK_PANIC);

        /* battery state */
        res = ptmuInit();

        if (R_FAILED(res))
            svcBreak(USERBREAK_PANIC);

        /* wifi state */
        res = acInit();

        if (R_FAILED(res))
            svcBreak(USERBREAK_PANIC);

        /* wireless */
        SOCKET_BUFFER = (u32 *)memalign(SO_BUF_ALIGN, SO_MAX_BUFSIZE);
        SOCKET_INITED = socInit(SOCKET_BUFFER, SO_MAX_BUFSIZE);

        HIDUSER_EnableAccelerometer();

        /* so we can have preemptive threads */
        APT_SetAppCpuTimeLimit(30);
    }

    void userAppExit()
    {
        HIDUSER_DisableAccelerometer();

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
}
