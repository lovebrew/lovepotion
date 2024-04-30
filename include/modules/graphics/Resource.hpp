#pragma once

#include <stddef.h>

namespace love
{
    class Resource
    {
      public:
        virtual ~Resource()
        {}

        virtual ptrdiff_t getHandle() const = 0;
    };
} // namespace love
