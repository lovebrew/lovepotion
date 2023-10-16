#include <common/exception.hpp>

#include <objects/imagerasterizer/imagerasterizer.hpp>

#include <objects/imagedata_ext.hpp>

#include <utilities/shaper/genericshaper.hpp>

#include <string.h>

using namespace love;

ImageRasterizer::ImageRasterizer(ImageData<Console::Which>* data, const uint32_t* glyphs,
                                 int glyphCount, int extraSpacing, float dpiScale) :
    imageData(data),
    glyphCount(glyphCount + 1),
    extraSpacing(extraSpacing)
{
    this->dpiScale = dpiScale;

    if (data->GetFormat() != PIXELFORMAT_RGBA8_UNORM)
        throw love::Exception("Only 32-bit RGBA images are supported in Image Fonts!");

    this->Load(glyphs, glyphCount);
}

int ImageRasterizer::GetLineHeight() const
{
    return this->GetHeight();
}

int ImageRasterizer::GetGlyphSpacing(uint32_t glyph) const
{
    auto it = this->glyphIndicies.find(glyph);
    if (it == this->glyphIndicies.end())
        return 0;

    return this->imageGlyphs[it->second].width + this->extraSpacing;
}

int ImageRasterizer::GetGlyphIndex(uint32_t glyph) const
{
    auto it = this->glyphIndicies.find(glyph);
    if (it == this->glyphIndicies.end())
        return 0;

    return it->second;
}

GlyphData* ImageRasterizer::GetGlyphDataForIndex(int index) const
{
    GlyphData::GlyphMetrics glyphMetrics {};
    uint32_t glyph = 0;

    if (index >= 0 && index < (int)this->imageGlyphs.size())
    {
        glyphMetrics.width   = this->imageGlyphs[index].width;
        glyphMetrics.advance = this->imageGlyphs[index].width + this->extraSpacing;
        glyph                = this->imageGlyphs[index].glyph;
    }

    glyphMetrics.height = this->metrics.height;

    auto* glyphData = new GlyphData(glyph, glyphMetrics, PIXELFORMAT_RGBA8_UNORM);

    if (glyphMetrics.width == 0)
        return glyphData;

    std::unique_lock lock(this->imageData->GetMutex());

    auto* pixels       = (Color32*)glyphData->GetData();
    const auto* source = (const Color32*)this->imageData->GetData();

    const auto size  = glyphData->GetWidth() * glyphData->GetHeight();
    const auto width = this->imageData->GetWidth();

    for (int idx = 0; idx < size; idx++)
    {
        const auto add = (width * (idx / glyphMetrics.width));
        auto color     = source[this->imageGlyphs[index].x + (idx % glyphMetrics.width) + add];

        if (color == this->spacer)
            pixels[idx] = Color32(0, 0, 0, 0);
        else
            pixels[idx] = color;
    }

    return glyphData;
}

void ImageRasterizer::Load(const uint32_t* glyphs, int glyphCount)
{
    const auto* pixels = (const Color32*)this->imageData->GetData();

    const auto width  = this->imageData->GetWidth();
    const auto height = this->imageData->GetHeight();

    std::unique_lock lock(this->imageData->GetMutex());

    this->metrics.height = height;
    this->spacer         = pixels[0];

    int start = 0;
    int end   = 0;

    {
        ImageGlyphData nullGlyph {};
        nullGlyph.x     = 0;
        nullGlyph.width = 0;
        nullGlyph.glyph = 0;

        this->imageGlyphs.push_back(nullGlyph);
        this->glyphIndicies[0] = (int)this->imageGlyphs.size() - 1;
    }

    for (int index = 0; index < glyphCount; ++index)
    {
        start = end;

        while (start < width && pixels[start] == spacer)
            ++start;

        end = start;

        while (end < width && pixels[end] != spacer)
            ++end;

        if (start >= end)
            break;

        ImageGlyphData imageGlyph {};
        imageGlyph.x     = start;
        imageGlyph.width = end - start;
        imageGlyph.glyph = glyphs[index];

        this->imageGlyphs.push_back(imageGlyph);
        this->glyphIndicies[glyphs[index]] = (int)this->imageGlyphs.size() - 1;
    }
}

int ImageRasterizer::GetGlyphCount() const
{
    return this->glyphCount;
}

bool ImageRasterizer::HasGlyph(uint32_t glyph) const
{
    return this->glyphIndicies.find(glyph) != this->glyphIndicies.end();
}

Rasterizer::DataType ImageRasterizer::GetDataType() const
{
    return Rasterizer::DataType::DATA_IMAGE;
}

TextShaper* ImageRasterizer::NewTextShaper()
{
    return new GenericShaper(this);
}