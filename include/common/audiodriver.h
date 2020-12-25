#pragma once

#include "modules/thread/types/lock.h"
#include <exception>

#if defined (_3DS)
    typedef struct _AudioDriver {} AudioDriver;
#endif

namespace love::AudrenDriver
{
    extern love::thread::MutexRef mutex;
    extern AudioDriver driver;

    inline bool driverInit = false;
    inline bool audioInit = false;

    void Initialize();

    void Update();

    void Exit();

    #if defined (_3DS)
        extern LightEvent ndspEvent;

        void Callback(void * data);
    #endif

    template <typename T>
    void LockFunction(const T & func)
    {
        try
        {
            thread::Lock lock(mutex);
            func(&driver);
        }
        catch(const std::exception & e)
        {
            throw;
        }
    }
}
