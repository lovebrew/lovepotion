#include "modules/thread/Thread.hpp"
#include "modules/thread/Threadable.hpp"

namespace love
{
    Thread::Thread(Threadable* threadable) : threadable(threadable), running(false), thread {}
    {}

    Thread::~Thread()
    {
        if (this->thread.joinable())
            this->thread.join();
    }

    void Thread::wait()
    {
        {
            std::unique_lock lock(this->mutex);

            if (!this->thread.joinable())
                return;
        }

        this->thread.join();
        this->running = false;
    }

    bool Thread::start()
    {
        std::unique_lock lock(this->mutex);

        if (this->running)
            return false;

        if (this->thread.joinable())
            this->thread.join();

        this->threadable->retain();

        this->thread  = std::thread(&love::Thread::runner, this);
        this->running = this->thread.joinable();

        if (!this->running)
            this->threadable->release();

        return this->running;
    }

    int Thread::runner(void* data)
    {
        auto* self = (love::Thread*)data;

        self->threadable->run();
        self->running = false;

        self->threadable->release();

        return 0;
    }

    bool Thread::isRunning()
    {
        return this->running;
    }
} // namespace love
