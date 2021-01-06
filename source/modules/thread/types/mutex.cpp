#include "modules/thread/types/mutex.h"

using namespace love::thread;

love::thread::Mutex::Mutex()
{
    LOVE_mutexInit(&this->mutex);
}

love::thread::Mutex::~Mutex()
{}

void love::thread::Mutex::Lock()
{
    LOVE_mutexLock(&this->mutex);
    this->locked = true;
}

bool love::thread::Mutex::IsLocked()
{
    return this->locked;
}

void love::thread::Mutex::Unlock()
{
    LOVE_mutexUnlock(&this->mutex);
    this->locked = false;
}
