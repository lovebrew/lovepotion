#pragma once

#include "modules/thread/types/mutex.h"
#include "modules/thread/types/threadable.h"

namespace love
{
    namespace common
    {
        class Thread
        {
          public:
            Thread(Threadable* t);

            /* Detach the thread */
            virtual ~Thread();

            virtual bool Start() = 0;

            /*
            ** In SDL, SDL_WaitThread will wait
            ** for the thread to be finished and then
            ** SDL frees the pointer to the thread
            */
            virtual void Wait() = 0;

            bool IsRunning();

          protected:
            Threadable* t;
            bool running;

            bool hasThread;

            ::Thread thread;
            thread::Mutex mutex;

            static void Runner(void* data);

            static s32 GetCurrentThreadPriority();
        };
    } // namespace common
} // namespace love
