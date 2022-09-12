#include <coreinit/thread.h>

#include <utilities/threads/thread_ext.hpp>
#include <utilities/threads/threadable.hpp>

namespace love
{
    Thread<Console::CAFE>::Thread(Threadable* threadable) : Thread<Console::ALL>(threadable)
    {}

    int Thread<Console::CAFE>::Runner()
    {
        this->threadable->Retain();

        this->threadable->ThreadFunction();

        this->running = false;
        this->threadable->Release();

        return 0;
    }

    bool Thread<Console::CAFE>::Start()
    {
        std::unique_lock lock(this->mutex);

        if (this->running)
            return false;

        if (this->thread.joinable())
            this->thread.join();

        this->thread  = love::thread(&Thread<Console::CAFE>::Runner, this);
        this->running = this->thread.joinable();

        return this->running;
    }
} // namespace love
