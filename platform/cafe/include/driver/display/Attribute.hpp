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

        GX2Attribute(uint32_t index, int components, GX2AttribFormat format, uint32_t offset);

        constexpr std::strong_ordering operator<=>(const GX2Attribute& other) const noexcept = default;

        bool operator==(const GX2Attribute& other);

        operator GX2AttribStream()
        {
            return this->stream;
        }

        bool isEnabled() const
        {
            return this->enabled;
        }

        void update(uint32_t index, int components, GX2AttribFormat format, uint32_t offset);

        void setBuffer(uint32_t buffer);

        void setEnabled(bool enabled);

        void setDivisor(uint32_t divisor);

      private:
        GX2AttribStream stream;
        bool enabled;
    };
} // namespace love
