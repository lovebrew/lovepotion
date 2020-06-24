#include "common/runtime.h"
#include "modules/thread/types/conditional.h"

using namespace love::thread;

Conditional::Conditional()
{
    LightSemaphore_Init(&this->cond.wait, 0, INT16_MAX);
    LightSemaphore_Init(&this->cond.signal, 0, INT16_MAX);

    this->cond->waiters = 0;
}

Conditional::~Conditional()
{

}

void Conditional::Signal()
{
    if (this->cond.waiters > 0)
    {
        LightSemaphore_Release(&this->cond.wait, 1);
        LightSemaphore_Acquire(&this->cond.signal, 1);

        --this->cond.waiters;
    }
}

void Conditional::Broadcast()
{
    if (this->cond.waiters > 0)
    {
        LightSemaphore_Release(&this->cond.wait, this->cond.waiters);
        LightSemaphore_Acquire(&this->cond.signal, this->cond.waiters);

        this->cond.waiters = 0;
    }
}

bool Conditional::Wait(thread::Mutex * _mutex, s64 timeout)
{
    thread::Mutex * mutex = (thread::Mutex *)_mutex;

    ++this->cond.waiters;

    LOVE_mutexUnlock(mutex);

    if (timeout <= 0)
        LightSemaphore_Acquire(&this->cond.wait, 1);
    else
    {
        bool finished;
        do
        {
            double start = love::Timer::GetTime() * 1000;
            finished = !LightSemaphore_TryAcquire(&cond->wait, 1);
            double stop  = love::Timer::GetTime() * 1000;
            timeout -= (stop - start);
        } while (timeout > 0 && !finished);

        if (!finished)
            return -1;
    }

    LightSemaphore_Release(&this->cond.signal, 1);

    LOVE_mutexLock(mutex);

    return true;
}