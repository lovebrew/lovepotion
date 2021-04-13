#include "modules/thread/types/lock.h"

using namespace love::thread;

Lock::Lock(Mutex& mutex) : mutex(&mutex)
{
    this->mutex->Lock();
}

Lock::Lock(Mutex* mutex) : mutex(mutex)
{
    this->mutex->Lock();
}

Lock::Lock(Lock&& other)
{
    this->mutex = other.mutex;
    other.mutex = nullptr;
}

Lock::~Lock()
{
    if (this->mutex)
        this->mutex->Unlock();
}
