#pragma once

#include "modules/thread/types/mutex.h"

namespace love::thread
{
    class Lock
    {
      public:
        Lock(Mutex& mutex);
        Lock(Mutex* mutex);

        Lock(Lock&& other);

        ~Lock();

      private:
        Mutex* mutex;
    };
} // namespace love::thread
