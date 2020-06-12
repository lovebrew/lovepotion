#pragma once

#if defined (_3DS)
    #include <3ds.h>

    typedef LightLock LOVE_Mutex;

    #define LOVE_mutexInit LightLock_Init
    #define LOVE_mutexLock LightLock_Lock
    #define LOVE_mutexUnlock LightLock_Unlock
#elif defined (__SWITCH__)
    #include <switch.h>

    typedef Mutex LOVE_Mutex;

    #define LOVE_mutexInit mutexInit
    #define LOVE_mutexLock mutexLock
    #define LOVE_mutexUnlock mutexUnlock
#endif

namespace love::thread
{
    class Mutex
    {
        public:
            Mutex();
            ~Mutex();

            void Lock();
            void Unlock();

        private:
            LOVE_Mutex mutex;
            Mutex(const Mutex &) {}
    };

    class MutexRef
    {
        public:
            MutexRef();
            ~MutexRef();

            operator Mutex * () const;
            Mutex * operator -> () const;

        private:
            Mutex * mutex;
    };

    inline Mutex * NewMutex()
    {
        return new Mutex();
    }
}
