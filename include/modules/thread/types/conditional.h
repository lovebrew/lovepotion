#pragma once

#include "modules/thread/types/mutex.h"

#if defined (_3DS)
    #include <3ds.h>

#elif defined (__SWITCH__)
    #include <switch.h>
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