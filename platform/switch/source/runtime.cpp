#include "common/runtime.h"
#include "common/assets.h"

extern "C"
{
    void userAppInit()
    {
        // Initialize libnx stuff

        Result res = plInitialize();

        if (R_FAILED(res))
            fatalThrow(res);

        res = nifmInitialize(NifmServiceType_User);

        if (R_FAILED(res))
            fatalThrow(res);

        res = accountInitialize(AccountServiceType_Application);

        if (R_FAILED(res))
            fatalThrow(res);

        res = setInitialize();

        if (R_FAILED(res))
            fatalThrow(res);

        res = psmInitialize();

        if (R_FAILED(res))
            fatalThrow(res);

        res = socketInitializeDefault();

        if (R_FAILED(res))
            fatalThrow(res);

        // Initialize everything else
        TTF_Init();
    }

    void userAppExit()
    {
        // Deinitialize libnx stuff
        if (Assets::IsFused())
            romfsExit();

        socketExit();
        psmExit();
        setExit();
        accountExit();
        nifmExit();
        plExit();

        // Deinitialize everything else
        TTF_Quit();
    }
}
