#include "common/runtime.h"
#include "objects/thread/thread.h"

bool LuaThread::Start(const vector<Variant> & args)
{
    this->args = args;

    s32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

    this->thread = threadCreate(TestRun, this, 0x1000, priority, -2, true);

    this->started = this->thread != NULL;

    return this->started;
}

void TestRun(void * thread)
{
    LuaThread * self = (LuaThread *)thread;
    self->Run();
}

void LuaThread::Wait()
{
    //threadWaitForExit(&this->thread);
}
