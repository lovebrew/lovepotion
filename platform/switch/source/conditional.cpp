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
    bool success = true;

    if (timeout < 0)
    {
        if (R_FAILED(condvarWait(&this->condVar, &_mutex->mutex)))
            success = false;
    }
    else if (R_FAILED(condvarWaitTimeout(&this->condVar, &_mutex->mutex, timeout)))
        success = false;

    return success;
}
