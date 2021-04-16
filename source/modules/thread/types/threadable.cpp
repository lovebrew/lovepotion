#include "modules/thread/types/threadable.h"
#include "thread/thread.h"

using namespace love;

love::Type Threadable::type("Threadable", &Object::type);

Threadable::Threadable()
{
    this->owner = newThread(this);
}

Threadable::~Threadable()
{
    delete this->owner;
}

bool Threadable::Start()
{
    return this->owner->Start();
}

void Threadable::Wait()
{
    this->owner->Wait();
}

bool Threadable::IsRunning() const
{
    return this->owner->IsRunning();
}

const char* Threadable::GetThreadName() const
{
    return this->threadName.empty() ? nullptr : this->threadName.c_str();
}
