#include "modules/event/event.h"

love::Event::Event()
{
    appletLockExit();
    appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
}

love::Event::~Event()
{
    appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleep);
    appletUnlockExit();
}