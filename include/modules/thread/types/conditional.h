#pragma once

#include "modules/thread/types/mutex.h"
#include "modules/timer/timer.h"

#if defined (_3DS)
    #include <3ds.h>

    struct LOVE_CondVar
    {
        LightSemaphore wait;
        LightSemaphore signal;
        LightLock mutex;

        LightEvent event;

        s32 waiters;
    };

    inline void LOVE_CreateCond(LOVE_CondVar * cond)
    {
        LightSemaphore_Init(&cond->wait, 0, INT16_MAX);
        LightSemaphore_Init(&cond->signal, 0, INT16_MAX);

        LightLock_Init(&cond->mutex);

        cond->waiters = 0;
    }

    inline void LOVE_CondSignal(LOVE_CondVar * cond)
    {
        LightLock_Lock(&cond->mutex);

        if (cond->waiters > 0)
        {
            LightSemaphore_Release(&cond->wait, 1);
            LightSemaphore_Acquire(&cond->signal, 1);

            --cond->waiters;
        }

        LightLock_Unlock(&cond->mutex);
    }

    inline void LOVE_CondBroadcast(LOVE_CondVar * cond)
    {
        LightLock_Lock(&cond->mutex);

        if (cond->waiters > 0)
        {
            LightSemaphore_Release(&cond->wait, cond->waiters);
            LightSemaphore_Acquire(&cond->signal, cond->waiters);

            cond->waiters = 0;
        }

        LightLock_Unlock(&cond->mutex);
    }

    inline Result LOVE_CondWait(LOVE_CondVar * cond, LOVE_Mutex * mutex)
    {
        LightLock_Lock(&cond->mutex);

        ++cond->waiters;

        LightLock_Unlock(&cond->mutex);

        LOVE_mutexUnlock(mutex);

        LightSemaphore_Acquire(&cond->wait, 1);

        LightSemaphore_Release(&cond->signal, 1);

        LOVE_mutexLock(mutex);

        return 0;
    }

    inline Result LOVE_CondWaitTimeout(LOVE_CondVar * cond, LOVE_Mutex * mutex, int timeout)
    {
        LightLock_Lock(&cond->mutex);

        ++cond->waiters;

        LightLock_Unlock(&cond->mutex);

        LOVE_mutexUnlock(mutex);

        // bool finished;
        // do
        // {
        //     double start = love::Timer::GetTime();
        //     finished = !LightSemaphore_TryAcquire(&cond->wait, 1);
        //     double stop  = love::Timer::GetTime();
        //     timeout -= (stop - start);
        // } while (timeout > 0 && !finished);

        // if (!finished)
        //     return -1;

        LightSemaphore_Release(&cond->signal, 1);

        LOVE_mutexLock(mutex);

        return 0;
    }
#elif defined (__SWITCH__)
    #include <switch.h>

    typedef CondVar LOVE_CondVar;

    #define LOVE_CreateCond(CondVar) condvarInit((CondVar))

    #define LOVE_CondSignal(CondVar) condvarWakeOne((CondVar))
    #define LOVE_CondBroadcast(CondVar) condvarWakeAll((CondVar))

    #define LOVE_CondWait(CondVar, mutex) condvarWait((CondVar), (mutex))
    #define LOVE_CondWaitTimeout(CondVar, mutex, timeout) condvarWaitTimeout((CondVar), (mutex), (timeout))
#endif

namespace love::thread
{
    class Conditional
    {
        public:
            Conditional();
            ~Conditional();

            void Signal();
            void Broadcast();
            bool Wait(Mutex * mutex, s64 timeout = -1);

        private:
            LOVE_CondVar condVar;
    };

    class ConditionalRef
    {
        public:
            ConditionalRef();
            ~ConditionalRef();

            operator Conditional * () const;
            Conditional * operator -> () const;

        private:
            Conditional * conditional;
    };

    inline Conditional * NewConditional()
    {
        return new Conditional();
    }
}