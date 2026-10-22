#pragma once

#include <condition_variable>
#include <mutex>

namespace love
{
    /* Equivalent to MutexRef in LÖVE. */
    using Mutex = std::recursive_mutex;
    using Lock  = std::unique_lock<Mutex>;

    class Conditional
    {
      public:
        void signal();
        void broadcast();

        bool wait(Mutex& mutex, int timeout = -1);

      private:
        std::condition_variable_any condition;
    };
} // namespace love
