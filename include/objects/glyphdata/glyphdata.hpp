#pragma once

#include <common/console.hpp>
#include <common/data.hpp>
#include <common/exception.hpp>
#include <common/pixelformat.hpp>

#include <utf8.h>

#include <memory>
#include <string>

namespace love
{
    class GlyphData : public Data
    {
      public:
        static Type type;

        struct GlyphMetrics
        {
            int width;
            int height;
            int advance;
            int bearingX;
            int bearingY;
        };

        GlyphData(uint32_t glyph, GlyphMetrics metrics, PixelFormat format);

        GlyphData(const GlyphData& other);

        ~GlyphData()
        {}

        virtual GlyphData* Clone() const override;

        virtual void* GetData() const override;

        virtual size_t GetSize() const override;

        PixelFormat GetFormat() const;

        size_t GetPixelSize() const;

        int GetHeight() const;

        int GetWidth() const;

        uint32_t GetGlyph() const;

        std::string GetGlyphString() const;

        int GetAdvance() const;

        int GetBearingX() const;

        int GetBearingY() const;

        int GetMinX() const;

        int GetMinY() const;

        int GetMaxX() const;

        int GetMaxY() const;

      protected:
        uint32_t glyph;
        GlyphMetrics metrics;
        std::unique_ptr<uint8_t> data;
        PixelFormat format;
    };
} // namespace love
