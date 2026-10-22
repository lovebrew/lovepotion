#include "common/Data.hpp"

namespace love
{
    Type Data::type("Data", &Object::type);

    Data::~Data()
    {}

    Mutex& Data::getMutex()
    {
        return this->mutex;
    }
} // namespace love
