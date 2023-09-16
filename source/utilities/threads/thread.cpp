#include <utilities/threads/thread.hpp>
#include <utilities/threads/threadable.hpp>

love::Thread::Thread(Threadable* threadable) : threadable(threadable), running(false), thread {}
{}

love::Thread::~Thread()
{
    if (this->thread.joinable())
        this->thread.join();
}

void love::Thread::Wait()
{
    {
        std::unique_lock lock(this->mutex);

        if (!this->thread.joinable())
            return;
    }

    this->thread.join();
    this->running = false;
}

bool love::Thread::Start()
{
    std::unique_lock lock(this->mutex);

    if (this->running)
        return false;

    if (this->thread)
        this->thread.join();

    this->threadable->Retain();

    this->thread  = love::thread(&love::Thread::Runner, this);
    this->running = this->thread.joinable();

    if (!this->running)
        this->threadable->Release();

    return this->running;
}

int love::Thread::Runner(void* data)
{
    love::Thread* self = (love::Thread*)data;

    self->threadable->ThreadFunction();
    self->running = false;

    self->threadable->Release();

    return 0;
}