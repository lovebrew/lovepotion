#pragma once

#include "modules/graphics/vertex.hpp"

#include <gx2/shaders.h>

#include <compare>
#include <cstdint>

namespace love
{
    class GX2Attribute
    {
      public:
        GX2Attribute();

        GX2Attribute(uint32_t location, int components, GX2AttribFormat format, uint32_t offset);

        bool operator==(const GX2Attribute& other) const;

        operator GX2AttribStream()
        {
            return this->stream;
        }

        GX2Attribute& setLocation(uint32_t location)
        {
            this->stream.location = location;
            return *this;
        }

        GX2Attribute& setFormat(GX2AttribFormat format)
        {
            this->stream.format = format;
            return *this;
        }

        GX2Attribute& setComponents(uint32_t components);

        GX2Attribute& setOffset(uint32_t offset)
        {
            this->stream.offset = offset;
            return *this;
        }

        GX2Attribute& setBuffer(uint32_t buffer)
        {
            this->stream.buffer = buffer;
            return *this;
        }

        GX2Attribute& setDivisor(uint32_t divisor)
        {
            this->stream.aluDivisor = divisor;
            return *this;
        }

      private:
        GX2AttribStream stream;
    };
} // namespace love
