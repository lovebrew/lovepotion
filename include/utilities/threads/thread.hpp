#pragma once

#include <common/console.hpp>
#include <utilities/threads/threads.hpp>

#include <atomic>

namespace love
{
    class Threadable;

    class Thread
    {
      public:
        Thread(Threadable* threadable);

        virtual ~Thread();

        bool IsRunning() const
        {
            return this->running;
        }

        void Wait();

        bool Start();

      private:
        static int Runner(void* data);

        Threadable* threadable;
        std::atomic<bool> running;

        love::thread thread;
        love::mutex mutex;
    };
} // namespace love
