#include "objects/truetyperasterizer/truetyperasterizer.h"

#include "common/bidirectionalmap.h"
#include "deko3d/graphics.h"

using namespace love;

TrueTypeRasterizer::TrueTypeRasterizer(FT_Library library, love::Data* data, int size,
                                       Hinting hinting) :
    data(data),
    hinting(hinting)
{
    /* dpiScale is 1.0f */
    size = floorf(size * this->dpiScale + 0.5f);

    if (size <= 0)
        throw love::Exception("Invalid TrueType font size: %d", size);

    FT_Error err = FT_Err_Ok;
    err          = FT_New_Memory_Face(library, (const FT_Byte*)data->GetData(), data->GetSize(), 0,
                                      &this->face);

    if (err != FT_Err_Ok)
        throw love::Exception(
            "TrueType Font loading error: FT_New_Face failed: 0x%x (problem with font file?)", err);

    err = FT_Set_Pixel_Sizes(this->face, size, size);

    if (err != FT_Err_Ok)
    {
        FT_Done_Face(this->face);
        throw love::Exception(
            "TrueType Font loading error: FT_Set_Pixel_Sizes failed: 0x%x (invalid size?)", err);
    }

    // Set global metrics
    FT_Size_Metrics& s    = this->face->size->metrics;
    this->metrics.advance = (int)(s.max_advance >> 6);
    this->metrics.ascent  = (int)(s.ascender >> 6);
    this->metrics.descent = (int)(s.descender >> 6);
    this->metrics.height  = (int)(s.height >> 6);
}

TrueTypeRasterizer::~TrueTypeRasterizer()
{
    FT_Done_Face(this->face);
}

int TrueTypeRasterizer::GetLineHeight() const
{
    return (int)(this->GetHeight() * 1.25);
}

GlyphData* TrueTypeRasterizer::GetGlyphData(uint32_t glyph) const
{
    love::GlyphData::GlyphMetrics glyphMetrics = {};
    FT_Glyph ftGlyph;

    FT_Error error     = FT_Err_Ok;
    FT_UInt loadOption = HintingToLoadOption(this->hinting);

    FT_UInt charIndex = FT_Get_Char_Index(this->face, glyph);
    error             = FT_Load_Glyph(this->face, charIndex, FT_LOAD_DEFAULT | loadOption);

    if (error != FT_Err_Ok)
        throw love::Exception("TrueType Font glyph error: FT_Load_Glyph failed (0x%x)", error);

    error = FT_Get_Glyph(this->face->glyph, &ftGlyph);

    if (error != FT_Err_Ok)
        throw love::Exception("TrueType Font glyph error: FT_Get_Glyph failed (0x%x)", error);

    FT_Render_Mode rendermode = FT_RENDER_MODE_NORMAL;
    if (this->hinting == HINTING_MONO)
        rendermode = FT_RENDER_MODE_MONO;

    error = FT_Glyph_To_Bitmap(&ftGlyph, rendermode, 0, 1);

    if (error != FT_Err_Ok)
        throw love::Exception("TrueType Font glyph error: FT_Glyph_To_Bitmap failed (0x%x)", error);

    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)ftGlyph;
    FT_Bitmap& bitmap           = bitmap_glyph->bitmap; // just to make things easier

    // Get metrics
    glyphMetrics.bearingX = bitmap_glyph->left;
    glyphMetrics.bearingY = bitmap_glyph->top;
    glyphMetrics.height   = bitmap.rows;
    glyphMetrics.width    = bitmap.width;
    glyphMetrics.advance  = (int)(ftGlyph->advance.x >> 16);

    // LOVE says it's LA8 pixel format
    love::GlyphData* glyphData = new love::GlyphData(glyph, glyphMetrics);

    const uint8_t* pixels = bitmap.buffer;
    uint8_t* dest         = (uint8_t*)glyphData->GetData();

    /*
    ** We treat the luminance of the FreeType bitmap
    ** as alpha in the GlyphData.
    */
    if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
    {
        for (int y = 0; y < (int)bitmap.rows; y++)
        {
            for (int x = 0; x < (int)bitmap.width; x++)
            {
                // Extract the 1-bit value and convert it to uint8.
                uint8_t v = ((pixels[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;

                dest[4 * (y * bitmap.width + x) + 0] = v;
                dest[4 * (y * bitmap.width + x) + 1] = v;
                dest[4 * (y * bitmap.width + x) + 2] = v;
                dest[4 * (y * bitmap.width + x) + 3] = v ? 255 : 0;
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
                dest[4 * (y * bitmap.width + x) + 0] = 255;
                dest[4 * (y * bitmap.width + x) + 1] = 255;
                dest[4 * (y * bitmap.width + x) + 2] = 255;
                dest[4 * (y * bitmap.width + x) + 3] = pixels[x];
            }

            pixels += bitmap.pitch;
        }
    }
    else
    {
        delete glyphData;
        FT_Done_Glyph(ftGlyph);
        throw love::Exception("Unknown TrueType glyph pixel mode.");
    }

    // Having copied the data over, we can destroy the glyph.
    FT_Done_Glyph(ftGlyph);

    return glyphData;
}

int TrueTypeRasterizer::GetGlyphCount() const
{
    return (int)this->face->num_glyphs;
}

bool TrueTypeRasterizer::HasGlyph(uint32_t glyph) const
{
    return FT_Get_Char_Index(this->face, glyph) != 0;
}

float TrueTypeRasterizer::GetKerning(uint32_t leftglyph, uint32_t rightglyph) const
{
    FT_Vector kerning = {};

    FT_UInt leftChar  = FT_Get_Char_Index(face, leftglyph);
    FT_UInt rightChar = FT_Get_Char_Index(face, rightglyph);

    FT_Get_Kerning(this->face, leftChar, rightChar, FT_KERNING_DEFAULT, &kerning);

    return float(kerning.x >> 6);
}

Rasterizer::DataType TrueTypeRasterizer::GetDataType() const
{
    return DATA_TRUETYPE;
}

bool TrueTypeRasterizer::Accepts(FT_Library library, love::Data* data)
{
    const FT_Byte* fbase = (const FT_Byte*)data->GetData();
    FT_Long fsize        = (FT_Long)data->GetSize();

    // Pasing in -1 for the face index lets us test if the data is valid.
    return FT_New_Memory_Face(library, fbase, fsize, -1, nullptr) == 0;
}

FT_UInt TrueTypeRasterizer::HintingToLoadOption(Hinting hint)
{
    switch (hint)
    {
        case HINTING_NORMAL:
        default:
            return FT_LOAD_TARGET_NORMAL;
        case HINTING_LIGHT:
            return FT_LOAD_TARGET_LIGHT;
        case HINTING_MONO:
            return FT_LOAD_TARGET_MONO;
        case HINTING_NONE:
            return FT_LOAD_NO_HINTING;
    }
}

// clang-format off
constexpr auto hintings = BidirectionalMap<>::Create(
    "none",   TrueTypeRasterizer::Hinting::HINTING_NONE,
    "mono",   TrueTypeRasterizer::Hinting::HINTING_MONO,
    "light",  TrueTypeRasterizer::Hinting::HINTING_LIGHT,
    "normal", TrueTypeRasterizer::Hinting::HINTING_NORMAL
);
// clang-format on

bool TrueTypeRasterizer::GetConstant(const char* in, Hinting& out)
{
    return hintings.Find(in, out);
}

bool TrueTypeRasterizer::GetConstant(Hinting in, const char*& out)
{
    return hintings.ReverseFind(in, out);
}

std::vector<const char*> TrueTypeRasterizer::GetConstants(Hinting)
{
    return hintings.GetNames();
}
