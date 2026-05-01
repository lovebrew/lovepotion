#pragma once

#include "driver/display/Attribute.hpp"

#include <gx2/shaders.h>

#include <array>
#include <bitset>

namespace love
{
    struct GX2AttributeLayout
    {
      public:
        static constexpr size_t MAX_ATTRIBUTES = 0x10;

        GX2AttributeLayout();

        ~GX2AttributeLayout();

        void reset();

        void set(uint32_t index, const GX2Attribute& stream);

        void bind();

        void rebuild();

      private:
        void destroy();

        std::array<GX2Attribute, MAX_ATTRIBUTES> streams;
        GX2FetchShader fetchShader;
        void* program;
        bool dirty;
    };
} // namespace love
