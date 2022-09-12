#include <switch/kernel/thread.h>

#include <utilities/threads/thread_ext.hpp>
#include <utilities/threads/threadable.hpp>

namespace love
{
    Thread<Console::HAC>::Thread(Threadable* threadable) : Thread<Console::ALL>(threadable)
    {}

    int Thread<Console::HAC>::Runner()
    {
        this->threadable->Retain();

        this->threadable->ThreadFunction();

        this->running = false;
        this->threadable->Release();

        return 0;
    }

    bool Thread<Console::HAC>::Start()
    {
        std::unique_lock lock(this->mutex);

        if (this->running)
            return false;

        if (this->thread.joinable())
            this->thread.join();

        this->thread  = love::thread(&Thread<Console::HAC>::Runner, this);
        this->running = this->thread.joinable();

        return this->running;
    }
} // namespace love
