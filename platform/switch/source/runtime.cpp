#include "common/runtime.h"
#include "modules/love.h"

extern "C"
{
    void userAppInit()
    {
        /* system fonts */
        Result res = plInitialize(PlServiceType_User);

        if (R_FAILED(res))
            fatalThrow(res);

        /* network interface manager */
        res = nifmInitialize(NifmServiceType_User);

        if (R_FAILED(res))
            fatalThrow(res);

        /* accounts */
        res = accountInitialize(AccountServiceType_Application);

        if (R_FAILED(res))
            fatalThrow(res);

        /* settings */
        res = setInitialize();

        if (R_FAILED(res))
            fatalThrow(res);

        /* battery */
        res = psmInitialize();

        if (R_FAILED(res))
            fatalThrow(res);

        /* wireless comms */
        res = socketInitializeDefault();

        if (R_FAILED(res))
            fatalThrow(res);
    }

    void userAppExit()
    {
        close(Love::debugSockfd);

        socketExit();
        psmExit();
        setExit();
        accountExit();
        nifmExit();
        plExit();
    }
}
