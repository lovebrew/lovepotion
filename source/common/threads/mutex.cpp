#include "common/runtime.h"
#include "common/threads/mutex.h"

using namespace love::thread;

Mutex::Mutex()
{
    LOVE_mutexInit(&this->mutex);
}

Mutex::~Mutex()
{}

void Mutex::Lock()
{
    LOVE_mutexLock(&this->mutex);
}

void Mutex::Unlock()
{
    LOVE_mutexUnlock(&this->mutex);
}
