#include "modules/thread/types/threadable.h"
#include "modules/thread/thread.h"

#include "modules/thread/types/lock.h"
#include "common/exception.h"

using namespace love;

LOVE_Thread::LOVE_Thread(Threadable * t) : t(t),
                                           running(false),
                                           hasThread(false)
{}

LOVE_Thread::~LOVE_Thread()
{
    if (this->hasThread)
        LOVE_CloseThread(this->thread);
}

void LOVE_Thread::Runner(void * data)
{
    LOVE_Thread * self = (LOVE_Thread *)data;
    self->t->Retain();

    self->t->ThreadFunction();

    {
        thread::Lock lock(self->mutex);
        self->running = false;
    }

    self->t->Release();
}

bool LOVE_Thread::IsRunning()
{
    thread::Lock lock(this->mutex);

    return this->running;
}

void LOVE_Thread::Wait()
{
    {
        thread::Lock lock(this->mutex);

        if (!this->hasThread)
            return;
    }

    LOVE_WaitThread(this->thread);
}

bool LOVE_Thread::Start()
{
    thread::Lock lock(this->mutex);

    if (this->running)
        return false;

    /*
    ** LÖVE calls SDL_WaitThread(thread, retVal)
    ** This is our equivalent
    */
    if (this->hasThread)
        LOVE_WaitThread(this->thread);

    s32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

    #if defined (_3DS)
        this->thread = threadCreate(Runner, this, 0x1000, priority - 1, 1, false);
        this->hasThread = (this->thread != NULL);
    #elif defined (__SWITCH__)
        Result created = threadCreate(&this->thread, Runner, this, NULL, 0x1000, 0x3B, 0);

        if (R_SUCCEEDED(created))
        {
            threadStart(&this->thread);
            this->hasThread = true;
        }
        else
            throw love::Exception("Failed to spawn thread");
    #endif

    this->running = this->hasThread;
    return this->running;
}
