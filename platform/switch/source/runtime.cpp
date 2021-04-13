#include <switch.h>

#include "common/results.h"

extern "C" {
void userAppInit()
{
    /* system fonts */
    R_ABORT_UNLESS(plInitialize(PlServiceType_User));

    /* network interface manager */
    R_ABORT_UNLESS(nifmInitialize(NifmServiceType_User));

    /* accounts */
    R_ABORT_UNLESS(accountInitialize(AccountServiceType_Application));

    /* settings */
    R_ABORT_UNLESS(setInitialize());

    /* system settings */
    R_ABORT_UNLESS(setsysInitialize());

    /* battery */
    R_ABORT_UNLESS(psmInitialize());

    /* wireless comms */
    R_ABORT_UNLESS(socketInitializeDefault());

    /* initialize controllers -- 4 players max */
    padConfigureInput(4, HidNpadStyleSet_NpadStandard);

    /* initialize touch screen */
    hidInitializeTouchScreen();
}

void userAppExit()
{
    socketExit();

    psmExit();

    setsysExit();

    setExit();

    accountExit();

    nifmExit();

    plExit();
}
}
