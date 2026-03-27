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
        static constexpr auto TESSELATION_NONE     = GX2_FETCH_SHADER_TESSELLATION_NONE;
        static constexpr auto TESSELATION_DISCRETE = GX2_TESSELLATION_MODE_DISCRETE;

        static constexpr size_t MAX_ATTRIBUTES = 0x10;

        GX2AttributeLayout();

        ~GX2AttributeLayout();

        void reset();

        void set(uint32_t index, const GX2Attribute& stream);

        void bind();

      private:
        void destroy();

        void rebuild();

        std::array<GX2Attribute, MAX_ATTRIBUTES> streams;
        GX2FetchShader fetchShader;
        void* program;
        bool dirty;
    };
} // namespace love
