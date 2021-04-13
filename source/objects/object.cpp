#include "objects/object.h"

using namespace love;

love::Type Object::type("Object", nullptr);

Object::Object(const Object& other) : count(1)
{}

Object::Object() : count(1)
{}

Object::~Object()
{}

int Object::GetReferenceCount() const
{
    return count;
}

/*
** @func Retain
** Adds 1 to the reference count
*/
void Object::Retain()
{
    this->count.fetch_add(1, std::memory_order_relaxed);
}

/*
** @func Release
** Remove 1 from the reference count
** If none remain, delet this
*/
void Object::Release()
{
    if (this->count.fetch_sub(1, std::memory_order_release) == 1)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete this;
    }
}
