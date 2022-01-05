#pragma once

#include "objects/object.h"

namespace love
{
    class CompressedMemory : public Object
    {
      public:
        CompressedMemory(size_t size);

        virtual ~CompressedMemory();

        uint8_t* data;
        size_t size;
    };
} // namespace love
