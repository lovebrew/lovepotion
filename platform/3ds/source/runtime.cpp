#include <3ds.h>
#include <malloc.h>

#include "common/results.h"

#define SOC_BUFSIZE  0x100000
#define BUFFER_ALIGN 0x1000

u32* SOCKET_BUFFER = nullptr;

extern "C"
{
    void userAppInit()
    {
        osSetSpeedupEnable(true);

        /* raw battery info */
        R_ABORT_UNLESS(mcuHwcInit());

        /* battery state */
        R_ABORT_UNLESS(ptmuInit());

        /* region info and fonts */
        R_ABORT_UNLESS(cfguInit());

        /* wifi state */
        R_ABORT_UNLESS(acInit());

        /* friends */
        R_ABORT_UNLESS(frdInit());

        /* wireless */
        SOCKET_BUFFER = (u32*)memalign(BUFFER_ALIGN, SOC_BUFSIZE);
        R_ABORT_LAMBDA_UNLESS(socInit(SOCKET_BUFFER, SOC_BUFSIZE), [&]() { free(SOCKET_BUFFER); });

        R_ABORT_UNLESS(y2rInit());

        /* accelerometer */
        HIDUSER_EnableAccelerometer();

        /* gyroscope */
        HIDUSER_EnableGyroscope();

        /* "enable" preemptive threads */
        APT_SetAppCpuTimeLimit(30);
    }

    void userAppExit()
    {
        y2rExit();

        HIDUSER_DisableGyroscope();

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
