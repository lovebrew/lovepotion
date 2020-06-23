#include "common/runtime.h"
#include "modules/thread/types/conditional.h"

using namespace love::thread;

Conditional::Conditional()
{
    LOVE_CreateCond(&this->condVar);
}

Conditional::~Conditional()
{

}

void Conditional::Signal()
{
    LOVE_CondSignal(&this->condVar);
}

void Conditional::Broadcast()
{
    LOVE_CondBroadcast(&this->condVar);
}

bool Conditional::Wait(thread::Mutex * _mutex, s64 timeout)
{
    Mutex * mutex = (Mutex *)_mutex;

    if (timeout < 0)
        return R_SUCCEEDED(LOVE_CondWait(&this->condVar, &mutex->mutex, 0));
    else
        return R_SUCCEEDED(LOVE_CondWaitTimeout(&this->condVar, &mutex->mutex, timeout));
}