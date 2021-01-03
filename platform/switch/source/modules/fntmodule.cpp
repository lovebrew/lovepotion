#include "modules/modfont/fntmodule.h"

#include "common/data.h"

#include "utf8/utf8.h"

using namespace love;

class SystemFontData : public love::Data
{
    public:
        SystemFontData(Font::SystemFontType type) : type(type)
        {
            plGetSharedFontByType(&this->fontData, (PlSharedFontType)type);
        }

        Data * Clone()   const override { return new SystemFontData(this->type); }
        void * GetData() const override { return this->fontData.address;         }
        size_t GetSize() const override { return this->fontData.size;            }

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

Rasterizer * FontModule::NewRasterizer(love::FileData * data)
{
    if (love::TrueTypeRasterizer::Accepts(this->library, data))
        return this->NewTrueTypeRasterizer(data, 12, love::TrueTypeRasterizer::HINTING_NORMAL);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
}

Rasterizer * FontModule::NewTrueTypeRasterizer(Data * data, int size, love::TrueTypeRasterizer::Hinting hinting)
{
    return new love::TrueTypeRasterizer(this->library, data, size, hinting);
}

Rasterizer * FontModule::NewTrueTypeRasterizer(Data * data, int size, float dpiScale, love::TrueTypeRasterizer::Hinting hinting)
{
    return new love::TrueTypeRasterizer(this->library, data, size, hinting);
}

/* default system font stuff */
Rasterizer * FontModule::NewTrueTypeRasterizer(int size, TrueTypeRasterizer::Hinting hinting)
{
    love::StrongReference<SystemFontData> data(new SystemFontData(Font::SystemFontType::TYPE_STANDARD), Acquire::NORETAIN);
    return NewTrueTypeRasterizer(data.Get(), size, hinting);
}

Rasterizer * FontModule::NewTrueTypeRasterizer(size_t size, float dpiScale, TrueTypeRasterizer::Hinting hinting)
{
    StrongReference<SystemFontData> data(new SystemFontData(Font::SystemFontType::TYPE_STANDARD), Acquire::NORETAIN);
    return NewTrueTypeRasterizer(data.Get(), size, dpiScale, hinting);
}

/* glyph data */
GlyphData * FontModule::NewGlyphData(Rasterizer * rasterizer, const std::string & text)
{
    uint32_t codepoint = 0;

    try
    {
        codepoint = utf8::peek_next(text.begin(), text.end());
    }
    catch (utf8::exception & e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return rasterizer->GetGlyphData(codepoint);
}

GlyphData * FontModule::NewGlyphData(Rasterizer * rasterizer, uint32_t glyph)
{
    return rasterizer->GetGlyphData(glyph);
}