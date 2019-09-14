#include "common/runtime.h"
#include "objects/thread/thread.h"

bool LuaThread::Start(const vector<Variant> & args)
{
    this->args = args;

    Result success = threadCreate(&this->thread, TestRun, this, 0x1000, 0x2C, -2);

    this->started = (success == 0);

    return this->started;
}

void TestRun(void * thread)
{
    LuaThread * self = (LuaThread *)thread;
    self->Run();
}

void LuaThread::Wait()
{
    threadWaitForExit(&this->thread);
}
