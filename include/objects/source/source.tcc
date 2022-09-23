#pragma once

#include <common/console.hpp>
#include <common/object.hpp>

#include <vector>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Source : public Object
    {
      public:
        enum SourceType
        {
            TYPE_STATIC,
            TYPE_STREAM,
            TYPE_QUEUE,
            TYPE_MAX_ENUM
        };

        enum Unit
        {
            UNIT_SECONDS,
            UNIT_SAMPLES,
            UNIT_MAX_ENUM
        };

        static inline Type type = Type("Source", &Object::type);

        Source(SourceType type) : sourceType(type)
        {}

        SourceType GetType() const
        {
            return this->sourceType;
        }

        static bool GetConstant(const char* in, SourceType& out);
        static bool GetConstant(SourceType in, const char*& out);
        static std::vector<const char*> GetConstants(SourceType);

        static bool GetConstant(const char* in, Unit& out);
        static bool GetConstant(Unit in, const char*& out);
        static std::vector<const char*> GetConstants(Unit);

      protected:
        SourceType sourceType;
    };
} // namespace love
