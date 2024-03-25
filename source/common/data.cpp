#include "common/Data.hpp"

namespace love
{
    Type Data::type("Data", &Object::type);

    Data::~Data()
    {
        if (this->mutex)
            delete this->mutex;
    }

    static void createMutex(std::mutex** address)
    {
        *address = new std::mutex();
    }

    std::mutex* Data::getMutex()
    {
        std::call_once(this->mutexCreated, createMutex, &this->mutex);
        return this->mutex;
    }
} // namespace love
