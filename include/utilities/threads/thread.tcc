#pragma once

#include <common/console.hpp>
#include <utilities/threads/threads.hpp>

#include <atomic>

namespace love
{
    class Threadable;

    template<Console::Platform T = Console::ALL>
    class Thread
    {
      public:
        Thread(Threadable* threadable) : threadable(threadable), running(false)
        {}

        virtual ~Thread()
        {}

        bool IsRunning() const
        {
            return this->running;
        }

        void Wait()
        {
            {
                std::unique_lock lock(this->mutex);

                if (!this->thread.joinable())
                    return;
            }

            this->thread.join();
            this->running = false;
        }

        void Runner(void* data);

      protected:
        Threadable* threadable;
        std::atomic<bool> running;

        love::thread thread;
        love::mutex mutex;
    };
} // namespace love
