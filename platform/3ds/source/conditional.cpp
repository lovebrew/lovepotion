#include "common/runtime.h"
#include "modules/thread/types/conditional.h"

using namespace love::thread;

Conditional::Conditional()
{
    LightSemaphore_Init(&this->condVar.wait, 0, INT16_MAX);
    LightSemaphore_Init(&this->condVar.signal, 0, INT16_MAX);

    this->condVar.waiters = 0;
}

Conditional::~Conditional()
{}

void Conditional::Signal()
{
    if (this->condVar.waiters > 0)
    {
        LightSemaphore_Release(&this->condVar.wait, 1);
        LightSemaphore_Acquire(&this->condVar.signal, 1);

        --this->condVar.waiters;
    }
}

void Conditional::Broadcast()
{
    if (this->condVar.waiters > 0)
    {
        LightSemaphore_Release(&this->condVar.wait, this->condVar.waiters);
        LightSemaphore_Acquire(&this->condVar.signal, this->condVar.waiters);

        this->condVar.waiters = 0;
    }
}

bool Conditional::Wait(thread::Mutex * _mutex, s64 timeout)
{
    bool success = true;
    ++this->condVar.waiters;

    LOVE_mutexUnlock(&_mutex->mutex);

    if (timeout < 0)
        LightSemaphore_Acquire(&this->condVar.wait, 1);
    else
    {
        do
        {
            double start = love::Timer::GetTime() * 1000;
            success = !LightSemaphore_TryAcquire(&this->condVar.wait, 1);
            double stop  = love::Timer::GetTime() * 1000;
            timeout -= (stop - start);
        } while (timeout > 0 && !success);
    }

    LightSemaphore_Release(&this->condVar.signal, 1);

    LOVE_mutexLock(&_mutex->mutex);

    return success;
}
