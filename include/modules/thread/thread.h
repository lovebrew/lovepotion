#pragma once

#include "modules/thread/types/mutex.h"
#include "modules/thread/types/threadable.h"

#if defined(_3DS)
    #define LOVE_WaitThread(thread)  threadJoin((thread), U64_MAX)
    #define LOVE_CloseThread(thread) threadFree((thread))
#elif defined(__SWITCH__)
    #define LOVE_WaitThread(thread)  threadWaitForExit(&(thread))
    #define LOVE_CloseThread(thread) threadClose(&(thread))
#endif

namespace love
{
    class LOVE_Thread
    {
      public:
        LOVE_Thread(Threadable* t);
        ~LOVE_Thread();

        bool Start();

        void Wait();

        bool IsRunning();

      private:
        Threadable* t;
        bool running;

        bool hasThread;

        Thread thread;
        thread::Mutex mutex;

        static void Runner(void* data);
    };

    inline LOVE_Thread* newThread(Threadable* t)
    {
        return new LOVE_Thread(t);
    }
} // namespace love