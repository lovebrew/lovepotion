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

        APT_SetAppCpuTimeLimit(0x1E);
    }

    void userAppExit()
    {
        mcuHwcExit();

        frdExit();

        cfguExit();

        gfxExit();

        osSetSpeedupEnable(false);
    }
}
