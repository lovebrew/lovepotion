#include "common/Map.hpp"
#include "modules/font/Font.tcc"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#if defined(__SWITCH__)
    #include <switch.h>
using SystemFontType = PlSharedFontType;
#elif defined(__WIIU__)
    #include <coreinit/memory.h>
using SystemFontType = OSSharedDataType;
#else
    #error "Unsupported platform for FreeType"
#endif

namespace love
{
    class FontModule : public FontModuleBase
    {
      public:
        FontModule();

        virtual ~FontModule();

        virtual Rasterizer* newRasterizer(FileData* data) const override;

        Rasterizer* newTrueTypeRasterizer(Data* data, int size,
                                          const Rasterizer::Settings& settings) const override;

        using FontModuleBase::newTrueTypeRasterizer;

        static ByteData* loadSystemFontByType(SystemFontType type);

        // clang-format off
#if defined(__SWITCH__)
        STRINGMAP_DECLARE(SystemFonts, PlSharedFontType,
          { "standard",                    PlSharedFontType_Standard             },
          { "simplified chinese",          PlSharedFontType_ChineseSimplified    },
          { "simplified chinese extended", PlSharedFontType_ExtChineseSimplified },
          { "traditional chinese",         PlSharedFontType_ChineseTraditional   },
          { "korean",                      PlSharedFontType_KO                   },
          { "nintendo extended",           PlSharedFontType_NintendoExt          }
        );
#elif defined(__WIIU__)
        STRINGMAP_DECLARE(SystemFonts, OSSharedDataType,
          { "standard",  OS_SHAREDDATATYPE_FONT_STANDARD  },
          { "chinese",   OS_SHAREDDATATYPE_FONT_CHINESE   },
          { "korean",    OS_SHAREDDATATYPE_FONT_KOREAN    },
          { "taiwanese", OS_SHAREDDATATYPE_FONT_TAIWANESE }
        );
#endif
        // clang-format on

      private:
        FT_Library library;
    };
} // namespace love
