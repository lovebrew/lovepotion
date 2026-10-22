#pragma once

#include "modules/thread/threads.hpp"

#include <thread>

namespace love
{
    class Threadable;

    class Thread
    {
      public:
        Thread(Threadable* threadable);

        virtual ~Thread();

        bool start();

        void wait();

        bool isRunning();

      private:
        static int runner(void* data);

        Threadable* threadable;
        std::atomic<bool> running;

        std::thread thread;
        Mutex mutex;
    };
} // namespace love
