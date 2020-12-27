#include <switch.h>
#include "common/debugger.h"

extern "C"
{
    void userAppInit()
    {
        /* system fonts */
        Result res = plInitialize(PlServiceType_User);

        if (R_FAILED(res))
            diagAbortWithResult(res);

        /* network interface manager */
        res = nifmInitialize(NifmServiceType_User);

        if (R_FAILED(res))
            diagAbortWithResult(res);

        /* accounts */
        res = accountInitialize(AccountServiceType_Application);

        if (R_FAILED(res))
            diagAbortWithResult(res);

        /* settings */
        res = setInitialize();

        if (R_FAILED(res))
            diagAbortWithResult(res);

        /* battery */
        res = psmInitialize();

        if (R_FAILED(res))
            diagAbortWithResult(res);

        /* wireless comms */
        res = socketInitializeDefault();

        if (R_FAILED(res))
            diagAbortWithResult(res);

        padConfigureInput(4, HidNpadStyleSet_NpadStandard);
        hidInitializeTouchScreen();
    }

    void userAppExit()
    {
        close(love::Debugger::Instance().GetSockfd());

        socketExit();
        psmExit();
        setExit();
        accountExit();
        nifmExit();
        plExit();
    }
}
