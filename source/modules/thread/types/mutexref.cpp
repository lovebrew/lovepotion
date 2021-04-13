#include "modules/thread/types/mutex.h"

using namespace love::thread;

MutexRef::MutexRef() : mutex(NewMutex())
{}

MutexRef::~MutexRef()
{
    delete this->mutex;
}

MutexRef::operator Mutex*() const
{
    return this->mutex;
}

love::thread::Mutex* MutexRef::operator->() const
{
    return this->mutex;
}
