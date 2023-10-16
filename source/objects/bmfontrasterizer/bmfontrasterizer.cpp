#include <objects/bmfontrasterizer/bmfontrasterizer.hpp>

#include <modules/filesystem/filesystem.hpp>
#include <modules/image/imagemodule.hpp>

#include <objects/imagedata_ext.hpp>

#include <utilities/functions.hpp>
#include <utilities/shaper/genericshaper.hpp>

namespace
{
    class BMFontLine
    {
      public:
        BMFontLine(const std::string& line);

        const std::string& GetTag() const
        {
            return this->tag;
        }

        int GetAttributeInt(const char* name) const;

        std::string GetAttributeString(const char* name) const;

      private:
        std::string tag;
        std::unordered_map<std::string, std::string> attributes;
    };

    BMFontLine::BMFontLine(const std::string& line)
    {
        this->tag    = line.substr(0, line.find(' '));
        size_t start = 0;

        while (start < line.length())
        {
            const auto end = line.find('=', start);
            if (end == std::string::npos || end + 1 >= line.length())
                break;

            auto keyStart = line.rfind(' ', end);
            if (keyStart == std::string::npos)
                break;

            keyStart++;

            const auto key  = line.substr(keyStart, end - keyStart);
            auto valueStart = end + 1;
            auto valueEnd   = valueStart + 1;

            if (line[valueStart] == '"')
            {
                valueStart++;
                valueEnd = line.find('"', valueStart) - 1;
            }
            else
                valueEnd = line.find(' ', valueStart + 1) - 1;

            valueEnd              = std::min(valueEnd, line.length() - 1);
            this->attributes[key] = line.substr(valueStart, valueEnd - valueStart + 1);

            start = valueEnd + 1;
        }
    }

    int BMFontLine::GetAttributeInt(const char* name) const
    {
        auto iterator = this->attributes.find(name);
        if (iterator == this->attributes.end())
            return 0;

        return (int)std::strtol(iterator->second.c_str(), nullptr, 10);
    }

    std::string BMFontLine::GetAttributeString(const char* name) const
    {
        auto iterator = this->attributes.find(name);
        if (iterator == this->attributes.end())
            return "";

        return iterator->second;
    }
} // namespace

using namespace love;

BMFontRasterizer::BMFontRasterizer(FileData* fontDefinition,
                                   const std::vector<ImageData<Console::Which>*>& images,
                                   float dpiScale) :
    fontSize(0),
    unicode(false),
    lineHeight(0)
{
    this->dpiScale = dpiScale;

    const auto& filename = fontDefinition->GetFilename();
    auto slashPosition   = filename.rfind('/');

    if (slashPosition != std::string::npos)
        fontFolder = filename.substr(0, slashPosition);

    for (int index = 0; index < (int)images.size(); index++)
    {
        if (images[index]->GetFormat() != PIXELFORMAT_RGBA8_UNORM)
            throw love::Exception("Only 32-bit RGBA images are supported in BMFonts.");

        this->images[index] = images[index];
    }

    std::string config((const char*)fontDefinition->GetData(), fontDefinition->GetSize());
    this->ParseConfig(config);
}

void BMFontRasterizer::ParseConfig(const std::string& config)
{
    {
        BMFontCharacter nullChar {};
        nullChar.glyph = 0;
        nullChar.page  = -1;
        this->characters.push_back(nullChar);
        this->characterIndices[0] = this->characters.size() - 1;
    }

    std::string line {};
    std::stringstream stream(config);

    // TODO: fix love::get_line to auto update the stream position
    while (std::getline(stream, line))
    {
        BMFontLine bmLine(line);
        const auto& tag = bmLine.GetTag();

        if (tag == "info")
        {
            this->fontSize = bmLine.GetAttributeInt("size");
            this->unicode  = bmLine.GetAttributeInt("unicode") != 0;
        }
        else if (tag == "common")
        {
            this->lineHeight = bmLine.GetAttributeInt("lineHeight");
            metrics.ascent   = bmLine.GetAttributeInt("base");
        }
        else if (tag == "page")
        {
            const auto& page = bmLine.GetAttributeInt("id");
            auto filename    = bmLine.GetAttributeString("file");

            if (!this->fontFolder.empty())
                filename = this->fontFolder + "/" + filename;

            if (this->images[page].Get() == nullptr)
            {
                auto* filesystem  = Module::GetInstance<Filesystem>(Module::M_FILESYSTEM);
                auto* imageModule = Module::GetInstance<ImageModule>(Module::M_IMAGE);

                if (!filesystem)
                    throw love::Exception("Filesystem module not loaded!");

                if (!imageModule)
                    throw love::Exception("Image module not loaded!");

                StrongReference<FileData> data(filesystem->Read(filename.c_str()),
                                               Acquire::NORETAIN);

                ImageData<Console::Which>* image = imageModule->NewImageData(data.Get());
                if (image->GetFormat() != PIXELFORMAT_RGBA8_UNORM)
                    throw love::Exception("Only 32-bit RGBA images are supported in BMFonts.");

                this->images[page].Set(image, Acquire::NORETAIN);
            }
        }
        else if (tag == "char")
        {
            BMFontCharacter character {};
            const auto codepoint = bmLine.GetAttributeInt("id");

            character.glyph = codepoint;
            character.x     = bmLine.GetAttributeInt("x");
            character.y     = bmLine.GetAttributeInt("y");
            character.page  = bmLine.GetAttributeInt("page");

            character.metrics.width    = bmLine.GetAttributeInt("width");
            character.metrics.height   = bmLine.GetAttributeInt("height");
            character.metrics.bearingX = bmLine.GetAttributeInt("xoffset");
            character.metrics.bearingY = bmLine.GetAttributeInt("yoffset");
            character.metrics.advance  = bmLine.GetAttributeInt("xadvance");

            this->characters.push_back(character);
            this->characterIndices[codepoint] = (int)this->characters.size() - 1;
        }
        else
        {
            const auto first  = bmLine.GetAttributeInt("first");
            const auto second = bmLine.GetAttributeInt("second");

            uint64_t packedGlyphs        = ((uint64_t)first << 32) | (uint64_t)second;
            this->kernings[packedGlyphs] = bmLine.GetAttributeInt("amount");
        }
    }

    if (this->characters.size() == 0)
        throw love::Exception("Invalid BMFont file (no character definitions?)");

    bool guessHeight = this->lineHeight == 0;

    for (const auto& character : this->characters)
    {
        if (character.glyph == 0)
            continue;

        const auto width  = character.metrics.width;
        const auto height = character.metrics.height;

        if (!this->unicode && character.glyph > 127)
        {
            throw love::Exception(
                "Invalid BMFont character id (only unicode and ASCII are supported)");
        }

        if (character.page < 0 || this->images[character.page].Get() == nullptr)
            throw love::Exception("Invalid BMFont character page id: %d", character.page);

        const auto* data = this->images[character.page].Get();

        if (!data->Inside(character.x, character.y))
            throw love::Exception("Invalid coordinates for BMFont character %u.", character.glyph);

        if (width > 0 && !data->Inside(character.x + width - 1, character.y))
            throw love::Exception("Invalid width for BMFont character %u.", character.glyph);

        if (height > 0 && !data->Inside(character.x, character.y + height - 1))
            throw love::Exception("Invalid height for BMFont character %u.", character.glyph);

        if (guessHeight)
            this->lineHeight = std::max(this->lineHeight, character.metrics.height);
    }

    this->metrics.height = this->lineHeight;
}

int BMFontRasterizer::GetLineHeight() const
{
    return this->lineHeight;
}

int BMFontRasterizer::GetGlyphSpacing(uint32_t glyph) const
{
    const auto iterator = this->characterIndices.find(glyph);
    if (iterator == this->characterIndices.end())
        return 0;

    return this->characters[iterator->second].metrics.advance;
}

int BMFontRasterizer::GetGlyphIndex(uint32_t glyph) const
{
    const auto iterator = this->characterIndices.find(glyph);
    if (iterator == this->characterIndices.end())
        return 0;

    return iterator->second;
}

GlyphData* BMFontRasterizer::GetGlyphDataForIndex(int index) const
{
    if (index < 0 || index >= (int)this->characters.size())
        return new GlyphData(0, GlyphData::GlyphMetrics {}, PIXELFORMAT_RGBA8_UNORM);

    const auto& character = this->characters[index];
    const auto& imagePair = this->images.find(character.page);

    if (imagePair == this->images.end())
        return new GlyphData(character.glyph, GlyphData::GlyphMetrics {}, PIXELFORMAT_RGBA8_UNORM);

    const auto* image = imagePair->second.Get();
    auto* data        = new GlyphData(character.glyph, character.metrics, PIXELFORMAT_RGBA8_UNORM);

    const auto pixelSize = image->GetPixelSize();

    auto* pixels            = (uint8_t*)data->GetData();
    const auto* destination = (const uint8_t*)image->GetData();

    std::unique_lock lock(image->GetMutex());

    for (int y = 0; y < character.metrics.height; y++)
    {
        auto index = ((character.y + y) * image->GetWidth() + character.x) * pixelSize;
        std::memcpy(&pixels[y * character.metrics.width * pixelSize], &destination[index],
                    character.metrics.width * pixelSize);
    }

    return data;
}

int BMFontRasterizer::GetGlyphCount() const
{
    return (int)this->characters.size();
}

bool BMFontRasterizer::HasGlyph(uint32_t glyph) const
{
    return this->characterIndices.find(glyph) != this->characterIndices.end();
}

float BMFontRasterizer::GetKerning(uint32_t left, uint32_t right) const
{
    const auto packedGlyphs = ((uint64_t)left << 32) | (uint64_t)right;
    const auto iterator     = this->kernings.find(packedGlyphs);

    if (iterator != this->kernings.end())
        return iterator->second;

    return 0.0f;
}

Rasterizer::DataType BMFontRasterizer::GetDataType() const
{
    return Rasterizer::DATA_IMAGE;
}

TextShaper* BMFontRasterizer::NewTextShaper()
{
    return new GenericShaper(this);
}

bool BMFontRasterizer::Accepts(FileData* fontDefinition)
{
    const auto* data = (const char*)fontDefinition->GetData();
    return fontDefinition->GetSize() > 4 && std::memcmp(data, "info", 4) == 0;
}
