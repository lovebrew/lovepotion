#include "objects/thread/thread.h"
#include "thread/types/lock.h"

love::Thread::Thread(Threadable* t) : common::Thread(t)
{}

love::Thread::~Thread()
{
    if (this->thread)
        threadDetach(this->thread);
}

void love::Thread::Wait()
{
    {
        thread::Lock lock(this->mutex);

        if (!this->thread)
            return;
    }

    threadJoin(this->thread, U64_MAX);
    threadFree(this->thread);

    thread::Lock(this->mutex);

    this->running = false;
    this->thread  = nullptr;
}

bool love::Thread::Start()
{
    thread::Lock lock(this->mutex);

    if (this->running)
        return false;

    /* Clean up the old handle properly */

    if (this->thread)
    {
        threadJoin(this->thread, U64_MAX);
        threadFree(this->thread);
    }

    s32 priority = love::common::Thread::GetCurrentThreadPriority();

    /* do not detach because otherwise it cannot be freed or joined */
    this->thread = threadCreate(Runner, this, 0x1000, priority - 1, 1, false);

    this->running = (this->thread != nullptr);

    return this->running;
}
