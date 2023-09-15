#include <utilities/threads/threadable.hpp>

using namespace love;

Type Threadable::type("Threadable", &Object::type);

Threadable::Threadable()
{
    this->owner = new love::Thread(this);
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
    return this->name.empty() ? nullptr : this->name.c_str();
}
