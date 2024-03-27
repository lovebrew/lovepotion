#pragma once

#include "common/Object.hpp"

#include <mutex>
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
        std::mutex mutex;
    };
} // namespace love
