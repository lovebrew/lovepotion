#include <3ds.h>
#include <malloc.h>

#include "common/results.h"

u32* SOCKET_BUFFER;

#define SOC_BUFSIZE  0x100000
#define BUFFER_ALIGN 0x1000

extern "C"
{
    void userAppInit()
    {
        osSetSpeedupEnable(true);

        /* mcu:hwc for raw battery info */
        R_ABORT_UNLESS(mcuHwcInit());

        /* battery state */
        R_ABORT_UNLESS(ptmuInit());

        /* Regional Stuff and Fonts */
        R_ABORT_UNLESS(cfguInit());

        /* wifi state */
        R_ABORT_UNLESS(acInit());

        /* friends */
        R_ABORT_UNLESS(frdInit());

        /* wireless */
        SOCKET_BUFFER = (u32*)memalign(BUFFER_ALIGN, SOC_BUFSIZE);
        R_ABORT_LAMBDA_UNLESS(socInit(SOCKET_BUFFER, SOC_BUFSIZE), [&]() { free(SOCKET_BUFFER); });

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

        cfguExit();

        ptmuExit();

        mcuHwcExit();
    }
}
