#include "modules/font/GlyphData.hpp"
#include "common/Console.hpp"

#include "utf8.h"

namespace love
{
    Type GlyphData::type("GlyphData", &Data::type);

    GlyphData::GlyphData(uint32_t glyph, GlyphMetrics& metrics, PixelFormat format) :
        glyph(glyph),
        metrics(metrics),
        data(nullptr),
        format(format)
    {
        if (this->format != PIXELFORMAT_LA8_UNORM && this->format != PIXELFORMAT_RGBA8_UNORM)
            throw love::Exception("Invalid GlyphData pixel format.");

        if constexpr (!Console::is(Console::CTR))
        {
            if (this->metrics.width > 0 && this->metrics.height > 0)
                this->data = new uint8_t[metrics.width * metrics.height * getPixelSize()];
        }
    }

    GlyphData::GlyphData(uint32_t glyph, GlyphMetrics& metrics, PixelFormat format, GlyphSheet sheet) :
        glyph(glyph),
        metrics(metrics),
        data(nullptr),
        format(format),
        sheet(sheet)
    {}

    GlyphData::GlyphData(const GlyphData& other) :
        glyph(other.glyph),
        metrics(other.metrics),
        data(nullptr),
        format(other.format)
    {
        if constexpr (!Console::is(Console::CTR))
        {
            this->data = new uint8_t[metrics.width * metrics.height * getPixelSize()];
            std::memcpy(this->data, other.data, other.getSize());
        }
    }

    GlyphData::~GlyphData()
    {
        delete[] this->data;
    }

    GlyphData* GlyphData::clone() const
    {
        return new GlyphData(*this);
    }

    size_t GlyphData::getPixelSize() const
    {
        return getPixelFormatBlockSize(this->format);
    }

    void* GlyphData::getData(int x, int y) const
    {
        if (this->data == nullptr)
            return nullptr;

        return this->data + (y * this->getWidth() + x) * getPixelSize();
    }

    size_t GlyphData::getSize() const
    {
        return size_t(this->getWidth() * this->getHeight()) * getPixelSize();
    }

    std::string GlyphData::getGlyphString() const
    {
        char buffer[5] { 0 };
        ptrdiff_t length = 0;

        try
        {
            char* end = utf8::append(glyph, buffer);
            length    = end - buffer;
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: {:s}", e.what());
        }

        if (length < 0)
            return std::string {};

        return std::string(buffer, length);
    }
} // namespace love
