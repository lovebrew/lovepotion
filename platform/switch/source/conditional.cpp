#include "modules/thread/types/conditional.h"

using namespace love::thread;

Conditional::Conditional()
{
    condvarInit(&this->condVar);
}

Conditional::~Conditional()
{}

void Conditional::Signal()
{
    condvarWakeOne(&this->condVar);
}

void Conditional::Broadcast()
{
    condvarWakeAll(&this->condVar);
}

bool Conditional::Wait(thread::Mutex* _mutex, s64 timeout)
{
    if (timeout < 0)
        condvarWait(&this->condVar, &_mutex->mutex);
    else if (R_FAILED(condvarWaitTimeout(&this->condVar, &_mutex->mutex, timeout)))
        return false;

    return true;
}
