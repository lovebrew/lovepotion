#include <3ds.h>

#include <utilities/results.hpp>

#include <cstdlib>

#define SOC_BUFSIZE  0x100000
#define BUFFER_ALIGN 0x1000

uint32_t* SOCKET_BUFFER = nullptr;

extern "C"
{
    void userAppInit()
    {
        osSetSpeedupEnable(true);

        gfxInitDefault();

        R_ABORT_UNLESS(cfguInit());

        R_ABORT_UNLESS(frdInit());

        SOCKET_BUFFER = (uint32_t*)aligned_alloc(BUFFER_ALIGN, SOC_BUFSIZE);
        R_ABORT_LAMBDA_UNLESS(socInit(SOCKET_BUFFER, SOC_BUFSIZE), [&]() { free(SOCKET_BUFFER); });

        R_ABORT_UNLESS(ptmuInit());

        R_ABORT_UNLESS(mcuHwcInit());

        R_ABORT_UNLESS(HIDUSER_EnableAccelerometer());

        R_ABORT_UNLESS(HIDUSER_EnableGyroscope());

        APT_SetAppCpuTimeLimit(0x1E);
    }

    void userAppExit()
    {
        HIDUSER_DisableGyroscope();

        HIDUSER_DisableAccelerometer();

        mcuHwcExit();

        socExit();
        std::free(SOCKET_BUFFER);

        frdExit();

        cfguExit();

        gfxExit();

        osSetSpeedupEnable(false);
    }
}
