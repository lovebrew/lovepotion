#include <3ds.h>
#include <malloc.h>

#include "common/results.h"

u32 * SOCKET_BUFFER;
Result SOCKET_INITED;

#define SO_MAX_BUFSIZE 0x100000
#define SO_BUF_ALIGN 0x1000

extern "C"
{
    void userAppInit()
    {
        osSetSpeedupEnable(true);

        /* mcu:hwc for raw battery info */
        R_ABORT_UNLESS(mcuHwcInit());

        /* battery state */
        R_ABORT_UNLESS(ptmuInit());

        /* wifi state */
        R_ABORT_UNLESS(acInit());

        /* friends */
        R_ABORT_UNLESS(frdInit());

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

        socExit();

        free(SOCKET_BUFFER);

        frdExit();

        acExit();

        ptmuExit();

        mcuHwcExit();
    }
}
