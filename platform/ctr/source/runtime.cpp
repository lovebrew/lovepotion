#include <3ds.h>

#include <utilities/results.hpp>

extern "C"
{
    void userAppInit()
    {
        osSetSpeedupEnable(true);

        gfxInitDefault();

        R_ABORT_UNLESS(cfguInit());

        R_ABORT_UNLESS(frdInit());

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

        frdExit();

        cfguExit();

        gfxExit();

        osSetSpeedupEnable(false);
    }
}
