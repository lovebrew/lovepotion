#pragma once

#include <stddef.h>

namespace love
{
    class Resource
    {
      public:
        virtual ~Resource();

        virtual ptrdiff_t GetHandle() const = 0;
    }
} // namespace love
