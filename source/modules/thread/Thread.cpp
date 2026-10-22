#include "modules/thread/Thread.hpp"
#include "modules/thread/Threadable.hpp"

namespace love
{
    Thread::Thread(Threadable* threadable) : threadable(threadable), running(false), thread {}
    {}

    Thread::~Thread()
    {
        if (this->thread.joinable())
            this->thread.detach();
    }

    void Thread::wait()
    {
        {
            Lock lock(this->mutex);

            if (!this->thread.joinable())
                return;
        }

        this->thread.join();
        this->running = false;
    }

    bool Thread::start()
    {
        Lock lock(this->mutex);

        if (this->running)
            return false;

        if (this->thread.joinable())
            this->thread.join();

        this->threadable->retain();
        this->running = true;

        try
        {
            this->thread = std::thread(&love::Thread::runner, this);
        }
        catch (...)
        {
            this->running = false;
            this->threadable->release();
            throw;
        }

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
