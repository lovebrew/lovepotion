#include "modules/audio/Source.hpp"

namespace love
{
    Type SourceBase::type("Source", &Object::type);

    SourceBase::SourceBase(Type sourceType) : sourceType(sourceType)
    {}

    SourceBase::~SourceBase()
    {}

    SourceBase::Type SourceBase::getType() const
    {
        return this->sourceType;
    }
} // namespace love
