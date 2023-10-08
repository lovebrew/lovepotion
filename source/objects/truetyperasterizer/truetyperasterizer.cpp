#include <common/exception.hpp>

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

#include <utilities/shaper/genericshaper.hpp>

#include <cmath>

using namespace love;

// clang-format off
static constexpr BidirectionalMap loadOptions = 
{
    TrueTypeRasterizer<>::HINTING_NORMAL, FT_LOAD_TARGET_NORMAL,
    TrueTypeRasterizer<>::HINTING_LIGHT,  FT_LOAD_TARGET_LIGHT,
    TrueTypeRasterizer<>::HINTING_MONO,   FT_LOAD_TARGET_MONO,
    TrueTypeRasterizer<>::HINTING_NONE,   FT_LOAD_NO_HINTING
};
// clang-format on

template<>
TrueTypeRasterizer<Console::Which>::TrueTypeRasterizer(FT_Library library, Data* data, int size,
                                                       float dpiScale,
                                                       TrueTypeRasterizer<>::Hinting hinting) :
    data(data),
    hinting(hinting)
{
    this->dpiScale = dpiScale;
    size           = std::floor(size * dpiScale + 0.5f);

    if (size <= 0)
        throw Exception("Invalid TrueType font size: %d", size);

    FT_Error error = FT_Err_Ok;
    error = FT_New_Memory_Face(library, (const FT_Byte*)data->GetData(), /* first byte in memory */
                               data->GetSize(),                          /* size in bytes        */
                               0,                                        /* face_index           */
                               &this->face);

    if (error != FT_Err_Ok)
        throw Exception(
            "TrueType Font loading error: FT_New_Face failed: 0x%x (problem with font file?)",
            error);

    error = FT_Set_Pixel_Sizes(face, size, size);

    if (error != FT_Err_Ok)
        throw Exception(
            "TrueType Font loading error: FT_Set_Pixel_Sizes failed: 0x%x (invalid size?)", error);

    // Set global metrics
    FT_Size_Metrics s = face->size->metrics;
    metrics.advance   = (int)(s.max_advance >> 6);
    metrics.ascent    = (int)(s.ascender >> 6);
    metrics.descent   = (int)(s.descender >> 6);
    metrics.height    = (int)(s.height >> 6);
}

template<>
TrueTypeRasterizer<Console::Which>::~TrueTypeRasterizer()
{
    FT_Done_Face(face);
}

template<>
TextShaper* TrueTypeRasterizer<Console::Which>::NewTextShaper()
{
    return new GenericShaper(this);
}

template<>
int TrueTypeRasterizer<Console::Which>::GetLineHeight() const
{
    return (int)(this->GetHeight() * 1.25);
}

template<>
bool TrueTypeRasterizer<Console::Which>::HasGlyph(uint32_t glyph) const
{
    return FT_Get_Char_Index(this->face, glyph) != 0;
}

template<>
int TrueTypeRasterizer<Console::Which>::GetGlyphSpacing(uint32_t glyph) const
{
    FT_Glyph ftglyph;
    FT_Error error = FT_Err_Ok;

    std::optional<long> loadoption = loadOptions.Find(this->hinting);

    const auto glyphIndex = FT_Get_Char_Index(this->face, glyph);
    error                 = FT_Load_Glyph(this->face, glyphIndex, FT_LOAD_DEFAULT | *loadoption);

    if (error != FT_Err_Ok)
        return 0;

    error = FT_Get_Glyph(this->face->glyph, &ftglyph);

    if (error != FT_Err_Ok)
        return 0;

    return (int)(ftglyph->advance.x >> 16);
}

template<>
int TrueTypeRasterizer<Console::Which>::GetGlyphIndex(uint32_t glyph) const
{
    const auto index      = FT_Get_Char_Index(this->face, glyph);
    this->glyphMap[glyph] = index;

    return this->glyphMap[glyph];
}

template<>
GlyphData* TrueTypeRasterizer<Console::Which>::GetGlyphDataForIndex(int index) const
{
    GlyphData::GlyphMetrics glyphMetrics = {};
    FT_Glyph ftglyph;

    FT_Error error                 = FT_Err_Ok;
    std::optional<long> loadoption = loadOptions.Find(this->hinting);

    error = FT_Load_Glyph(this->face, index, FT_LOAD_DEFAULT | *loadoption);

    if (error != FT_Err_Ok)
        throw Exception("TrueType Font glyph error: FT_Load_Glyph failed (0x%x)", error);

    error = FT_Get_Glyph(this->face->glyph, &ftglyph);

    if (error != FT_Err_Ok)
        throw Exception("TrueType Font glyph error: FT_Get_Glyph failed (0x%x)", error);

    FT_Render_Mode mode = FT_RENDER_MODE_NORMAL;
    if (this->hinting == HINTING_MONO)
        mode = FT_RENDER_MODE_MONO;

    error = FT_Glyph_To_Bitmap(&ftglyph, mode, 0, 1);

    if (error != FT_Err_Ok)
        throw Exception("TrueType Font glyph error: FT_Glyph_To_Bitmap failed (0x%x)", error);

    FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)ftglyph;
    const auto& bitmap         = bitmapGlyph->bitmap;

    glyphMetrics.width    = bitmap.width;
    glyphMetrics.height   = bitmap.rows;
    glyphMetrics.bearingX = bitmapGlyph->left;
    glyphMetrics.bearingY = bitmapGlyph->top;
    glyphMetrics.advance  = (int)(ftglyph->advance.x >> 16);

    GlyphData* glyphData =
        new GlyphData(this->glyphMap[index], glyphMetrics, PIXELFORMAT_RGBA8_UNORM);

    const uint8_t* pixels = bitmap.buffer;
    uint8_t* destination  = (uint8_t*)glyphData->GetData();

    /* force convert it to RGBA8 */
    if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
    {
        for (int y = 0; y < (int)bitmap.rows; y++)
        {
            for (int x = 0; x < (int)bitmap.width; x++)
            {
                uint8_t value = ((pixels[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;

                destination[4 * (y * bitmap.width + x) + 0] = value;
                destination[4 * (y * bitmap.width + x) + 1] = value;
                destination[4 * (y * bitmap.width + x) + 2] = value;
                destination[4 * (y * bitmap.width + x) + 3] = value ? 255 : 0;
            }
            pixels += bitmap.pitch;
        }
    }
    else if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
    {
        for (int y = 0; y < (int)bitmap.rows; y++)
        {
            for (int x = 0; x < (int)bitmap.width; x++)
            {
                destination[4 * (y * bitmap.width + x) + 0] = 255;
                destination[4 * (y * bitmap.width + x) + 1] = 255;
                destination[4 * (y * bitmap.width + x) + 2] = 255;
                destination[4 * (y * bitmap.width + x) + 3] = pixels[x];
            }
            pixels += bitmap.pitch;
        }
    }
    else
    {
        delete glyphData;
        FT_Done_Glyph(ftglyph);

        throw love::Exception("Unknown TrueType glyph pixel mode: %u", bitmap.pixel_mode);
    }

    FT_Done_Glyph(ftglyph);

    return glyphData;
}

template<>
int TrueTypeRasterizer<Console::Which>::GetGlyphCount() const
{
    return this->face->num_glyphs;
}

template<>
float TrueTypeRasterizer<Console::Which>::GetKerning(uint32_t left, uint32_t right) const
{
    FT_Vector kerning {};
    FT_Error error = FT_Err_Ok;

    const auto leftChar  = FT_Get_Char_Index(this->face, left);
    const auto rightChar = FT_Get_Char_Index(this->face, right);

    error = FT_Get_Kerning(this->face, leftChar, rightChar, FT_KERNING_DEFAULT, &kerning);

    if (error != FT_Err_Ok)
        return 0;

    return float(kerning.x >> 6);
}

template<>
Rasterizer::DataType TrueTypeRasterizer<Console::Which>::GetDataType() const
{
    return Rasterizer::DataType::DATA_TRUETYPE;
}

template<>
bool TrueTypeRasterizer<Console::Which>::Accepts(FT_Library library, Data* data)
{
    const FT_Byte* fbase = (const FT_Byte*)data->GetData();
    FT_Long fsize        = (FT_Long)data->GetSize();

    return FT_New_Memory_Face(library, fbase, fsize, -1, nullptr) == 0;
}