#include "common/Exception.hpp"

#include "modules/font/GenericShaper.hpp"
#include "modules/font/ImageRasterizer.hpp"

#include <algorithm>

namespace love
{
    static_assert(sizeof(Color32) == 4, "sizeof(Color32) must equal 4 bytes!");

    ImageRasterizer::ImageRasterizer(ImageData* data, const uint32_t* glyphs, int count, int spacing,
                                     float dpiScale) :
        data(data),
        count(count + 1),
        spacing(spacing)
    {
        this->dpiScale = dpiScale;

        if (data->getFormat() != PIXELFORMAT_RGBA8_UNORM)
            throw love::Exception("Only 32-bit RGBA images are supported in Image Fonts!");

        this->load(glyphs, count);
    }

    ImageRasterizer::~ImageRasterizer()
    {}

    int ImageRasterizer::getLineHeight() const
    {
        return this->getHeight();
    }

    int ImageRasterizer::getGlyphSpacing(uint32_t glyph) const
    {
        auto it = this->indices.find(glyph);
        if (it == this->indices.end())
            return 0;

        return this->glyphs[glyph].width + this->spacing;
    }

    int ImageRasterizer::getGlyphIndex(uint32_t glyph) const
    {
        auto it = this->indices.find(glyph);
        if (it == this->indices.end())
            return 0;

        return it->second;
    }

    GlyphData* ImageRasterizer::getGlyphDataForIndex(int index) const
    {
        GlyphMetrics metrics {};
        uint32_t glyph = 0;

        if (index >= 0 && index < (int)this->glyphs.size())
        {
            metrics.width   = this->glyphs[index].width;
            metrics.advance = this->glyphs[index].width + this->spacing;
            glyph           = this->glyphs[index].glyph;
        }

        metrics.height  = this->metrics.height;
        auto* glyphData = new GlyphData(glyph, metrics, PIXELFORMAT_RGBA8_UNORM);

        if (metrics.width == 0)
            return glyphData;

        Lock lock(this->data->getMutex());

        auto* dataPixels = (Color32*)glyphData->getData();
        auto* pixels     = (const Color32*)this->data->getData();

        for (int i = 0; i < glyphData->getWidth() * glyphData->getHeight(); i++)
        {
            const auto offset = this->data->getWidth() * (i / metrics.width);
            auto pixel        = pixels[this->glyphs[index].x + (i % metrics.width) + offset];

            if (pixel == this->spacer)
                dataPixels[i] = Color32(0, 0, 0, 0);
            else
                dataPixels[i] = pixel;
        }

        return glyphData;
    }

    void ImageRasterizer::load(const uint32_t* glyphs, int count)
    {
        auto pixels = (const Color32*)this->data->getData();

        int width  = this->data->getWidth();
        int height = this->data->getHeight();

        this->metrics.height = height;
        this->spacer         = pixels[0];

        int start = 0;
        int end   = 0;

        {
            ImageGlyphData null {};
            null.x     = 0;
            null.width = 0;
            null.glyph = 0;

            this->glyphs.push_back(null);
            this->indices[0] = (int)this->glyphs.size() - 1;
        }

        for (int i = 0; i < count; ++i)
        {
            start = end;

            while (start < width && pixels[start] == spacer)
                ++start;

            end = start;

            while (end < width && pixels[end] != spacer)
                ++end;

            if (start >= end)
                break;

            ImageGlyphData glyph {};
            glyph.x     = start;
            glyph.width = end - start;
            glyph.glyph = glyphs[i];

            this->glyphs.push_back(glyph);
            this->indices[glyphs[i]] = (int)this->glyphs.size() - 1;

            this->metrics.advance = std::max(this->metrics.advance, float(glyph.width + this->spacing));
        }
    }

    int ImageRasterizer::getGlyphCount() const
    {
        return this->count;
    }

    bool ImageRasterizer::hasGlyph(uint32_t glyph) const
    {
        return this->indices.find(glyph) != this->indices.end();
    }

    Rasterizer::DataType ImageRasterizer::getDataType() const
    {
        return DATA_IMAGE;
    }

    TextShaper* ImageRasterizer::newTextShaper()
    {
        return new GenericShaper(this);
    }
} // namespace love
