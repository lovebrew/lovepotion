#include "common/runtime.h"
#include "modules/font/fontmodule.h"

#include "common/data.h"

using namespace love;

class DefaultFontData : public love::Data
{
    public:
        DefaultFontData()
        { plGetSharedFontByType(&this->fontData, PlSharedFontType_Standard); }

        Data * Clone()   const override { return new DefaultFontData();  }
        void * GetData() const override { return this->fontData.address; }
        size_t GetSize() const override { return this->fontData.size;    }

    private:
        PlFontData fontData;
};

class SystemFontData : public love::Data
{
    public:
        SystemFontData(Font::SystemFontType type)
        {
            this->type = type;
            plGetSharedFontByType(&this->fontData, (PlSharedFontType)type);
        }

        Data * Clone()   const override { return new SystemFontData(this->type);   }
        void * GetData() const override { return this->fontData.address; }
        size_t GetSize() const override { return this->fontData.size; }

    private:
        PlFontData fontData;
        Font::SystemFontType type;
};

FontModule::FontModule()
{
    if (FT_Init_FreeType(&this->library))
        throw love::Exception("TrueTypeFont Loading error: FT_Init_FreeType failed");
}

FontModule::~FontModule()
{
    FT_Done_FreeType(this->library);
}

Data * FontModule::GetSystemFont(Font::SystemFontType type)
{
    return new SystemFontData(type);
}

love::Rasterizer * FontModule::NewRasterizer(love::FileData * data)
{
    if (love::TrueTypeRasterizer::Accepts(this->library, data))
        return this->NewTrueTypeRasterizer(data, 12, love::TrueTypeRasterizer::HINTING_NORMAL);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
}

Rasterizer * FontModule::NewTrueTypeRasterizer(size_t size, float dpiScale, TrueTypeRasterizer::Hinting hinting)
{}

Rasterizer * FontModule::NewTrueTypeRasterizer(void * data, size_t dataSize, int size, TrueTypeRasterizer::Hinting hinting)
{}

love::Rasterizer * FontModule::NewTrueTypeRasterizer(love::Data * data, int size, love::TrueTypeRasterizer::Hinting hinting)
{
    return new love::TrueTypeRasterizer(this->library, data, size, hinting);
}

love::Rasterizer * FontModule::NewTrueTypeRasterizer(int size, TrueTypeRasterizer::Hinting hinting)
{
    love::StrongReference<DefaultFontData> data(new DefaultFontData, Acquire::NORETAIN);
    return NewTrueTypeRasterizer(data.Get(), size, hinting);
}

GlyphData * FontModule::NewGlyphData(Rasterizer * rasterizer, const std::string & text)
{
    uint32_t codepoint = 0;
    auto bytes = decode_utf8(&codepoint, (uint8_t *)&text[0]);

    if (bytes < 0)
    {
        bytes = 1; // skip the invalid sequence
        codepoint = 0xFFFD;
    }

    return rasterizer->GetGlyphData(codepoint);
}