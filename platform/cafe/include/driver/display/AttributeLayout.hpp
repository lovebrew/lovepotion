#pragma once

#include "driver/display/Attribute.hpp"

#include <gx2/shaders.h>

#include <cstdint>
#include <vector>

namespace love
{
    struct GX2AttributeLayout
    {
      public:
        static constexpr size_t MAX_ATTRIBUTES = 0x10;

        GX2AttributeLayout();

        ~GX2AttributeLayout();

        void bind();

        void reset();

        void rebuild(const std::vector<GX2AttribStream>& state);

      private:
        void destroy();

        GX2FetchShader fetchShader;
        uint8_t* program;
        bool dirty;
    };
} // namespace love
