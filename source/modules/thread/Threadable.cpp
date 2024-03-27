#include "modules/thread/Threadable.hpp"

namespace love
{
    Type Threadable::type("Threadable", &Object::type);

    Threadable::Threadable()
    {
        this->owner = new Thread(this);
    }

    Threadable::~Threadable()
    {
        delete this->owner;
    }

    bool Threadable::start()
    {
        return this->owner->start();
    }

    void Threadable::wait()
    {
        this->owner->wait();
    }

    bool Threadable::isRunning()
    {
        return this->owner->isRunning();
    }

    const char* Threadable::getThreadName() const
    {
        return this->threadName.empty() ? nullptr : this->threadName.c_str();
    }
} // namespace love
