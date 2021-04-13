#include "modules/thread/types/conditional.h"

using namespace love::thread;

Conditional::Conditional()
{
    CondVar_Init(&this->condVar);
}

Conditional::~Conditional()
{}

void Conditional::Signal()
{
    CondVar_Signal(&this->condVar);
}

void Conditional::Broadcast()
{
    CondVar_Broadcast(&this->condVar);
}

bool Conditional::Wait(thread::Mutex* _mutex, s64 timeout)
{
    if (timeout < 0)
        CondVar_Wait(&this->condVar, &_mutex->mutex);
    else if (CondVar_WaitTimeout(&this->condVar, &_mutex->mutex, timeout))
        return false;

    return true;
}
