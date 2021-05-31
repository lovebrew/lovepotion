#include "modules/font/fontmodule.h"

#include "common/data.h"
#include "utf8/utf8.h"

using namespace love;

FontModule::FontModule()
{
    if (FT_Init_FreeType(&this->library))
        throw love::Exception("TrueTypeFont Loading error: FT_Init_FreeType failed");
}

FontModule::~FontModule()
{
    FT_Done_FreeType(this->library);
}

/* Create new System Font with Size and Hinting */
Rasterizer* FontModule::NewTrueTypeRasterizer(Font::SystemFontType fontType, int size,
                                              TrueTypeRasterizer::Hinting hinting)
{
    love::StrongReference<DefaultFontData> data(new DefaultFontData(fontType), Acquire::NORETAIN);

    return this->NewTrueTypeRasterizer(data.Get(), size, hinting);
}

/* Create new System Font with Size, DPI Scale, and Hinting */
Rasterizer* FontModule::NewTrueTypeRasterizer(Font::SystemFontType fontType, int size,
                                              float dpiScale, TrueTypeRasterizer::Hinting hinting)
{
    love::StrongReference<DefaultFontData> data(new DefaultFontData(fontType), Acquire::NORETAIN);

    return this->NewTrueTypeRasterizer(data.Get(), size, dpiScale, hinting);
}

/* Create new Standard System Font with Size */
Rasterizer* FontModule::NewTrueTypeRasterizer(int size, TrueTypeRasterizer::Hinting hinting)
{
    love::StrongReference<DefaultFontData> data(new DefaultFontData, Acquire::NORETAIN);

    return this->NewTrueTypeRasterizer(data.Get(), size, hinting);
}

/* Create new Standard System Font with Size and DPI Scale */
Rasterizer* FontModule::NewTrueTypeRasterizer(int size, float dpiScale,
                                              TrueTypeRasterizer::Hinting hinting)
{
    StrongReference<DefaultFontData> data(new DefaultFontData, Acquire::NORETAIN);

    return this->NewTrueTypeRasterizer(data.Get(), size, dpiScale, hinting);
}

/* Create from FileData */
Rasterizer* FontModule::NewRasterizer(FileData* data)
{
    if (TrueTypeRasterizer::Accepts(this->library, data))
        return this->NewTrueTypeRasterizer(data, 12, TrueTypeRasterizer::HINTING_NORMAL);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
}

/* Create from Data with Size and Hinting */
Rasterizer* FontModule::NewTrueTypeRasterizer(Data* data, int size,
                                              TrueTypeRasterizer::Hinting hinting)
{
    return this->NewTrueTypeRasterizer(data, size, 1.0f, hinting);
}

/* Create from Data with Size, DPI Scaling, and Hinting */
Rasterizer* FontModule::NewTrueTypeRasterizer(Data* data, int size, float dpiScale,
                                              love::TrueTypeRasterizer::Hinting hinting)
{
    return new TrueTypeRasterizer(this->library, data, size, hinting);
}
