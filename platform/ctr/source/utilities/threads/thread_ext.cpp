#include <utilities/threads/thread_ext.hpp>
#include <utilities/threads/threadable.hpp>

namespace love
{
    Thread<Console::CTR>::Thread(Threadable* threadable) : Thread<Console::ALL>(threadable)
    {}

    int Thread<Console::CTR>::Runner()
    {
        this->threadable->Retain();

        this->threadable->ThreadFunction();

        this->running = false;
        this->threadable->Release();

        return 0;
    }

    bool Thread<Console::CTR>::Start()
    {
        std::unique_lock lock(this->mutex);

        if (this->running)
            return false;

        if (this->thread)
            this->thread.join();

        int32_t priority = 0;
        svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

        auto metadata = thread::basic_meta;

        metadata.stack_size = 0x4000;
        metadata.prio       = priority - 1;
        metadata.core_id    = 0x01;

        this->thread = love::thread(metadata, &Thread<Console::CTR>::Runner, this);

        if (this->thread)
            this->running = true;

        return this->running;
    }
} // namespace love
