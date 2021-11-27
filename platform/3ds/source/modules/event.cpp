#include "modules/event/event.h"
#include "driver/hidrv.h"

#include "modules/thread/types/mutex.h"

static aptHookCookie s_aptHookCookie;

#define Instance() (love::Module::GetInstance<love::Event>(love::Module::M_EVENT))

static void aptEventHook(const APT_HookType type, void* param)
{
    switch (type)
    {
        case APTHOOK_ONRESTORE:
        case APTHOOK_ONWAKEUP:
            Instance()->GetDriver()->SendFocus(true);
            break;
        case APTHOOK_ONSUSPEND:
        case APTHOOK_ONSLEEP:
            Instance()->GetDriver()->SendFocus(false);
            break;
        case APTHOOK_ONEXIT:
            Instance()->GetDriver()->SendQuit();
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
