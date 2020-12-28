#include "modules/event/event.h"
#include "driver/hidrv.h"

#include "modules/thread/types/mutex.h"

static aptHookCookie s_aptHookCookie;

static void aptEventHook(const APT_HookType type, void * param)
{
    switch (type)
    {
        case APTHOOK_ONRESTORE:
        case APTHOOK_ONWAKEUP:
            love::driver::hidrv.SendFocus(true);
            break;
        case APTHOOK_ONSUSPEND:
        case APTHOOK_ONSLEEP:
            love::driver::hidrv.SendFocus(false);
            break;
        default:
            break;
    }
}

love::Event::Event()
{
    aptHook(&s_aptHookCookie, aptEventHook, nullptr);
}

love::Event::~Event()
{
    aptUnhook(&s_aptHookCookie);
}