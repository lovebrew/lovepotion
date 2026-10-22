#include "common/Exception.hpp"

#include "modules/filesystem/physfs/Filesystem.hpp"
#include "modules/font/BMFontRasterizer.hpp"
#include "modules/font/GenericShaper.hpp"
#include "modules/font/Rasterizer.hpp"
#include "modules/font/TextShaper.hpp"
#include "modules/image/Image.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

#include <cstdlib>
#include <cstring>

namespace love
{
    BMFontLine::BMFontLine(const std::string& line)
    {
        this->tag    = line.substr(0, line.find(' '));
        size_t start = 0;

        while (start < line.length())
        {
            size_t fpos = line.find('=', start);
            if (fpos == std::string::npos || fpos + 1 >= line.length())
                break;

            size_t keyStart = line.rfind(' ', fpos);
            if (keyStart == std::string::npos)
                break;

            keyStart++;

            auto key          = line.substr(keyStart, fpos - keyStart);
            size_t valueStart = fpos + 1;
            size_t valueEnd   = valueStart + 1;

            if (line[valueStart] == '"')
            {
                valueStart++;
                valueEnd = line.find('"', valueStart) - 1;
            }
            else
                valueEnd = std::min(valueEnd, line.length() - 1);

            this->attributes[key] = line.substr(valueStart, valueEnd - valueStart + 1);
            start                 = valueEnd + 1;
        }
    }

    int BMFontLine::getAttributeInt(const char* name) const
    {
        auto it = this->attributes.find(name);
        if (it == this->attributes.end())
            return 0;

        return (int)strtol(it->second.c_str(), nullptr, 10);
    }

    std::string BMFontLine::getAttributeString(const char* name) const
    {
        auto it = this->attributes.find(name);
        if (it == this->attributes.end())
            return std::string {};

        return it->second;
    }

    BMFontRasterizer::BMFontRasterizer(FileData* definition, const std::vector<ImageData*>& images,
                                       float dpiScale) :
        size(0),
        unicode(false),
        lineHeight(0)
    {
        this->dpiScale = dpiScale;

        const auto& filename = definition->getFilename();

        size_t separatorPosition = filename.rfind('/');
        if (separatorPosition != std::string::npos)
            this->folder = filename.substr(0, separatorPosition);

        for (int i = 0; i < (int)images.size(); i++)
        {
            if (images[i]->getFormat() != PIXELFORMAT_RGBA8_UNORM)
                throw love::Exception("Only 32-bit RGBA images are supported in BMFonts.");

            this->images[i] = images[i];
        }

        std::string config((const char*)definition->getData(), definition->getSize());
        this->parseConfig(config);
    }

    BMFontRasterizer::~BMFontRasterizer()
    {}

    void BMFontRasterizer::parseConfig(const std::string& config)
    {
        {
            BMFontCharacter null {};
            null.page  = -1;
            null.glyph = 0;
            this->characters.push_back(null);
            this->indices[0] = (int)this->characters.size() - 1;
        }

        std::stringstream stream(config);
        std::string line {};

        while (std::getline(stream, line))
        {
            BMFontLine configLine(line);
            const auto& tag = configLine.getTag();

            if (tag == "info")
            {
                this->size    = configLine.getAttributeInt("size");
                this->unicode = configLine.getAttributeInt("unicode") > 0;
            }
            else if (tag == "common")
            {
                this->lineHeight      = configLine.getAttributeInt("lineHeight");
                this->metrics.ascent  = configLine.getAttributeInt("base");
                this->metrics.descent = this->metrics.ascent - this->lineHeight;
            }
            else if (tag == "page")
            {
                int index     = configLine.getAttributeInt("id");
                auto filename = configLine.getAttributeString("file");

                if (!this->folder.empty())
                    filename = this->folder + "/" + filename;

                if (this->images[index].get() == nullptr)
                {
                    auto* filesystem  = Module::getInstance<Filesystem>(Module::M_FILESYSTEM);
                    auto* imageModule = Module::getInstance<Image>(Module::M_IMAGE);

                    if (!filesystem)
                        throw love::Exception("Cannot load BMFont: Filesystem module not loaded!");

                    if (!imageModule)
                        throw love::Exception("Cannot load BMFont: Image module not loaded!");

                    StrongRef<FileData> data(filesystem->read(filename.c_str()), Acquire::NO_RETAIN);
                    auto* imageData = imageModule->newImageData(data.get());

                    if (imageData->getFormat() != PIXELFORMAT_RGBA8_UNORM)
                    {
                        imageData->release();
                        throw love::Exception("Only 32-bit RGBA images are supported in BMFonts.");
                    }

                    this->images[index].set(imageData, Acquire::NO_RETAIN);
                }
            }
            else if (tag == "char")
            {
                BMFontCharacter character {};
                uint32_t id = (uint32_t)configLine.getAttributeInt("id");

                character.x    = configLine.getAttributeInt("x");
                character.y    = configLine.getAttributeInt("y");
                character.page = configLine.getAttributeInt("page");

                character.metrics.width    = configLine.getAttributeInt("width");
                character.metrics.height   = configLine.getAttributeInt("height");
                character.metrics.bearingX = configLine.getAttributeInt("xoffset");
                character.metrics.bearingY = configLine.getAttributeInt("yoffset");
                character.metrics.advance  = configLine.getAttributeInt("xadvance");

                character.glyph = id;

                this->characters.push_back(character);
                this->indices[id] = (int)this->characters.size() - 1;

                this->metrics.advance = std::max(this->metrics.advance, (float)character.metrics.advance);
            }
            else if (tag == "kerning")
            {
                auto first  = (uint32_t)configLine.getAttributeInt("first");
                auto second = (uint32_t)configLine.getAttributeInt("second");

                auto packed           = ((uint64_t)first << 32) | (uint64_t)second;
                this->kerning[packed] = configLine.getAttributeInt("amount");
            }
        }

        if (this->characters.size() == 0)
            throw love::Exception("Invalid BMFont: (no character definitions?)");

        bool guessHeight = this->lineHeight == 0;

        for (const auto& character : this->characters)
        {
            if (character.glyph == 0)
                continue;

            int width  = character.metrics.width;
            int height = character.metrics.height;

            if (!this->unicode && character.glyph > 127)
                throw love::Exception("Invalid BMFont character id (only unicode and ASCII are supported).");

            if (character.page < 0 || this->images[character.page] == nullptr)
                throw love::Exception("Invalid BMFont character page id: {:d}.", character.page);

            const auto* data = this->images[character.page].get();
            const auto glyph = character.glyph;

            if (!data->inside(character.x, character.y))
                throw love::Exception("Invalid coordinates for BMFont character {:u}.", glyph);

            if (width > 0 && !data->inside(character.x + width - 1, character.y))
                throw love::Exception("Invalid width {:d} for BMFont character {:u}.", width, glyph);

            if (height > 0 && !data->inside(character.x, character.y + height - 1))
                throw love::Exception("Invalid height {:d} for BMFont character {:u}.", height, glyph);

            if (guessHeight)
                this->lineHeight = std::max(this->lineHeight, character.metrics.height);
        }

        this->metrics.height = this->lineHeight;
    }

    int BMFontRasterizer::getLineHeight() const
    {
        return this->lineHeight;
    }

    int BMFontRasterizer::getGlyphSpacing(uint32_t glyph) const
    {
        auto it = this->indices.find(glyph);
        if (it == this->indices.end())
            return 0;

        return this->characters[it->second].metrics.advance;
    }

    int BMFontRasterizer::getGlyphIndex(uint32_t glyph) const
    {
        auto it = this->indices.find(glyph);
        if (it == this->indices.end())
            return 0;

        return it->second;
    }

    GlyphData* BMFontRasterizer::getGlyphDataForIndex(int index) const
    {
        if (index < 0 || index >= (int)this->characters.size())
            return new GlyphData(0, GlyphMetrics(), PIXELFORMAT_RGBA8_UNORM);

        const auto& character = this->characters[index];
        const auto& pair      = this->images.find(character.page);

        if (pair == this->images.end())
            return new GlyphData(character.glyph, GlyphMetrics(), PIXELFORMAT_RGBA8_UNORM);

        auto* image = pair->second.get();
        auto* glyph = new GlyphData(character.glyph, character.metrics, PIXELFORMAT_RGBA8_UNORM);

        size_t pixelSize = image->getPixelSize();

        uint8_t* pixels        = (uint8_t*)glyph->getData();
        const uint8_t* iPixels = (const uint8_t*)image->getData();

        Lock lock(image->getMutex());

        for (int y = 0; y < character.metrics.height; ++y)
        {
            size_t idIndex = ((character.y + y) * image->getWidth() + character.x) * pixelSize;
            size_t size    = character.metrics.width * pixelSize;
            std::memcpy(&pixels[y * character.metrics.width * pixelSize], &iPixels[idIndex], size);
        }

        return glyph;
    }

    int BMFontRasterizer::getGlyphCount() const
    {
        return (int)this->characters.size();
    }

    bool BMFontRasterizer::hasGlyph(uint32_t glyph) const
    {
        return this->indices.find(glyph) != this->indices.end();
    }

    float BMFontRasterizer::getKerning(uint32_t left, uint32_t right) const
    {
        auto packed = ((uint64_t)left << 32) | (uint64_t)right;

        auto it = this->kerning.find(packed);
        if (it != this->kerning.end())
            return it->second;

        return 0.0f;
    }

    Rasterizer::DataType BMFontRasterizer::getDataType() const
    {
        return DATA_IMAGE;
    }

    TextShaper* BMFontRasterizer::newTextShaper()
    {
        return new GenericShaper(this);
    }
} // namespace love
