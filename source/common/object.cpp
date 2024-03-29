#include <common/object.hpp>

using namespace love;

Type Object::type("Object", nullptr);

Object::Object() : count(1)
{}

Object::Object(const Object& /*other*/) : count(1)
{}

Object::~Object()
{}

int Object::GetReferenceCount() const
{
    return this->count;
}

void Object::Retain()
{
    this->count.fetch_add(1, std::memory_order_relaxed);
}

void Object::Release()
{
    if (this->count.fetch_sub(1, std::memory_order_release) == 1)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete this;
    }
}
