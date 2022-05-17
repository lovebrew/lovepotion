#pragma once

#include "deko3d/buffer.h"

namespace love
{
    class Mapper
    {
      public:
        Mapper(Buffer& buffer) : buffer(buffer)
        {
            this->elements = buffer.Map();
        }

        ~Mapper()
        {
            this->buffer.UnMap();
        }

        void* Get()
        {
            return this->elements;
        }

      private:
        Buffer& buffer;
        void* elements;
    };
} // namespace love
