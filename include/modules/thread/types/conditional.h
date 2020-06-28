#pragma once

#include "modules/thread/types/mutex.h"
#include "modules/timer/timer.h"
#include <atomic>

#if defined (_3DS)
    #include <3ds.h>

    struct LOVE_CondVar
    {
        LightSemaphore wait;
        LightSemaphore signal;

        LightEvent event;

        std::atomic<s32> waiters;
    };

    inline int LightSemaphore_TryAcquire(LightSemaphore* semaphore, s32 count)
    {
        s32 old_count;
        do
        {
            old_count = __ldrex(&semaphore->current_count);
            if (old_count < count)
            {
                __clrex();
                return 1; // failure
            }
        } while (__strex(&semaphore->current_count, old_count - count));

        return 0; // success
    }
#elif defined (__SWITCH__)
    #include <switch.h>

    typedef CondVar LOVE_CondVar;
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
