#include "common/Object.hpp"
#include "common/Exception.hpp"

namespace love
{
    Type Object::type("Object", nullptr);

    Object::Object() : count(1)
    {}

    Object::Object(const Object&) : count(1)
    {}

    Object::~Object()
    {}

    int Object::getReferenceCount() const
    {
        return this->count;
    }

    void Object::retain()
    {
        this->count.fetch_add(1, std::memory_order_relaxed);
    }

    void Object::release()
    {
        if (this->count.fetch_sub(1, std::memory_order_release) == 1)
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            delete this;
        }
    }
} // namespace love
