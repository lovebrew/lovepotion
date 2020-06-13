#pragma once

#include "modules/thread/types/mutex.h"

#if defined (_3DS)
    #include <3ds.h>

    typedef struct LOVE_CondVar {};

    #define LOVE_CreateCond(CondVar)

    #define LOVE_CondSignal(CondVar)
    #define LOVE_CondBroadcast(CondVar)

    #define LOVE_CondWait(CondVar, mutex) 0
    #define LOVE_CondWaitTimeout(CondVar, mutex, timeout) 0
#elif defined (__SWITCH__)
    #include <switch.h>

    typedef CondVar LOVE_CondVar;

    #define LOVE_CreateCond(CondVar) condvarInit(&(CondVar))

    #define LOVE_CondSignal(CondVar) condvarWakeOne(&(CondVar))
    #define LOVE_CondBroadcast(CondVar) condvarWakeAll(&(CondVar))

    #define LOVE_CondWait(CondVar, mutex) condvarWait(&(CondVar), &(mutex))
    #define LOVE_CondWaitTimeout(CondVar, mutex, timeout) condvarWaitTimeout(&(CondVar), &(mutex), (timeout))
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
            bool Wait(Mutex * mutex, int timeout = -1);

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