#pragma once

#include "common/Map.hpp"

#include "modules/filesystem/FileData.hpp"
#include "modules/font/Rasterizer.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace love
{
    class TrueTypeRasterizer : public Rasterizer
    {
      public:
        TrueTypeRasterizer(FT_Library library, Data* data, int size, const Settings& settings,
                           float defaultDPIScale);

        virtual ~TrueTypeRasterizer()
        {}

        // clang-format off
        STRINGMAP_DECLARE(Hintings, Hinting,
            { "normal", HINTING_NORMAL },
            { "light",  HINTING_LIGHT  },
            { "mono",   HINTING_MONO   },
            { "none",   HINTING_NONE   }
        );
        // clang-format on
    };
} // namespace love
