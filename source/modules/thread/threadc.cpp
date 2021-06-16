#include "modules/thread/threadc.h"
#include "modules/thread/types/threadable.h"

#include "common/exception.h"
#include "modules/thread/types/lock.h"

using namespace love;

love::common::Thread::Thread(Threadable* t) : t(t), running(false), hasThread(false), thread {}
{}

love::common::Thread::~Thread()
{}

void love::common::Thread::Runner(void* data)
{
    Thread* self = (Thread*)data;
    self->t->Retain();

    self->t->ThreadFunction();

    {
        thread::Lock lock(self->mutex);
        self->running = false;
    }

    self->t->Release();
}

s32 love::common::Thread::GetCurrentThreadPriority()
{
    s32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

    return priority;
}

bool love::common::Thread::IsRunning()
{
    thread::Lock lock(this->mutex);

    return this->running;
}
