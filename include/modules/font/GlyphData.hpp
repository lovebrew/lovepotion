#pragma once

#include "common/Data.hpp"
#include "common/Exception.hpp"
#include "common/Map.hpp"
#include "common/int.hpp"
#include "common/pixelformat.hpp"

#include <cstring>
#include <string>

namespace love
{
    struct GlyphMetrics
    {
        int height;
        int width;
        int advance;
        int bearingX;
        int bearingY;
    };

    struct GlyphSheet
    {
        int index;

        float left;
        float top;
        float right;
        float bottom;

        uint16_t width;
        uint16_t height;
        void* data;
    };

    class GlyphData : public Data
    {
      public:
        static Type type;

        GlyphData(uint32_t glyph, GlyphMetrics& metrics, PixelFormat format);

        GlyphData(uint32_t glyph, GlyphMetrics& metrics, PixelFormat format, GlyphSheet sheet);

        GlyphData(const GlyphData& other);

        ~GlyphData();

        GlyphData* clone() const override;

        void* getData() const
        {
            return this->data;
        }

        size_t getSize() const override;

        void* getData(int x, int y) const;

        size_t getPixelSize() const;

        virtual int getHeight() const
        {
            return this->metrics.height;
        }

        virtual int getWidth() const
        {
            return this->metrics.width;
        }

        uint32_t getGlyph() const
        {
            return this->glyph;
        }

        std::string getGlyphString() const;

        int getAdvance() const
        {
            return this->metrics.advance;
        }

        int getBearingX() const
        {
            return this->metrics.bearingX;
        }

        int getBearingY() const
        {
            return this->metrics.bearingY;
        }

        int getMinX() const
        {
            return this->getBearingX();
        }

        int getMaxX() const
        {
            return this->getBearingX() + this->getWidth();
        }

        int getMinY() const
        {
            return this->getBearingY();
        }

        int getMaxY() const
        {
            return this->getBearingY() + this->getHeight();
        }

        GlyphSheet getSheet()
        {
            return this->sheet;
        }

        PixelFormat getFormat() const
        {
            return this->format;
        }

      private:
        uint32_t glyph;
        GlyphMetrics metrics;

        uint8_t* data;

        PixelFormat format;
        GlyphSheet sheet;
    };
} // namespace love
