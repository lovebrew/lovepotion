#include <objects/truetyperasterizer.hpp>

#include <math.h>

using namespace love;

TrueTypeRasterizer::TrueTypeRasterizer(FT_Library library, Data* data, int size, float dpiScale,
                                       Hinting hinting) :
    data(data),
    hinting(hinting)
{
    this->dpiScale = dpiScale;
    size           = std::floor(size * dpiScale + 0.5f);

    if (size <= 0)
        throw love::Exception("Invalid TrueType font size: %d", size);

    FT_Error error = FT_Err_Ok;
    error = FT_New_Memory_Face(library, (const FT_Byte*)data->GetData(), data->GetSize(), 0,
                               &this->face);

    if (error != FT_Err_Ok)
        throw love::Exception("FT_New_Face error: %s", FT_Error_String(error));

    error = FT_Set_Pixel_Sizes(this->face, size, size);

    if (error != FT_Err_Ok)
        throw love::Exception("FT_Set_Pixel_Sizes error: %s", FT_Error_String(error));

    const auto& metrics   = this->face->size->metrics;
    this->metrics.advance = (int)(metrics.max_advance >> 6);
    this->metrics.ascent  = (int)(metrics.ascender >> 6);
    this->metrics.descent = (int)(metrics.descender >> 6);
    this->metrics.height  = (int)(metrics.height >> 6);

    this->dataType = DATA_TRUETYPE;
}

TrueTypeRasterizer::~TrueTypeRasterizer()
{
    FT_Done_Face(this->face);
}

int TrueTypeRasterizer::GetLineHeight() const
{
    return this->GetHeight() * 1.25f;
}

GlyphData* TrueTypeRasterizer::GetGlyphData(const std::string_view& text) const
{
    uint32_t codepoint = 0;

    try
    {
        codepoint = utf8::peek_next(text.begin(), text.end());
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return this->GetGlyphData(codepoint);
}

GlyphData* TrueTypeRasterizer::GetGlyphData(uint32_t glyph) const
{
    GlyphData::GlyphMetrics metrics {};
    FT_Glyph ftGlyph;

    /* load glyph information */

    FT_Error error        = FT_Err_Ok;
    const auto loadOption = *loadOptions.Find(this->hinting);

    const auto index = FT_Get_Char_Index(this->face, glyph);

    if (error = FT_Load_Glyph(this->face, index, FT_LOAD_DEFAULT | loadOption); error != FT_Err_Ok)
        throw love::Exception("FT_Load_Glyph error: %s", FT_Error_String(error));

    if (error = FT_Get_Glyph(this->face->glyph, &ftGlyph); error != FT_Err_Ok)
        throw love::Exception("FT_Get_Glyph error: %s", FT_Error_String(error));

    auto renderMode = FT_RENDER_MODE_NORMAL;

    if (this->hinting == HINTING_MONO)
        renderMode = FT_RENDER_MODE_MONO;

    if (error = FT_Glyph_To_Bitmap(&ftGlyph, renderMode, 0, 1); error != FT_Err_Ok)
        throw love::Exception("FT_Glyph_To_Bitmap error: %s", FT_Error_String(error));

    auto bitmapGlyph = (FT_BitmapGlyph)ftGlyph;
    auto& bitmap     = bitmapGlyph->bitmap;

    /* get the metrics */

    metrics.bearingX = bitmapGlyph->left;
    metrics.bearingY = bitmapGlyph->top;
    metrics.height   = bitmap.rows;
    metrics.width    = bitmap.width;
    metrics.advance  = (int)(ftGlyph->advance.x >> 16);

    /* copy the pixel data */
    GlyphData* glyphData = new GlyphData(glyph, metrics, PIXELFORMAT_LA8_UNORM);

    const uint8_t* pixels = bitmap.buffer;
    uint8_t* destination  = (uint8_t*)glyphData->GetData();

    if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
    {
        for (int y = 0; y < (int)bitmap.rows; y++)
        {
            for (int x = 0; x < (int)bitmap.width; x++)
            {
                uint8_t value = ((pixels[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;

                destination[2 * (y * bitmap.width + x) + 0] = 255;
                destination[2 * (y * bitmap.width + x) + 1] = value;
            }
            pixels += bitmap.width;
        }
    }
    else if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
    {
        for (int y = 0; y < (int)bitmap.rows; y++)
        {
            for (int x = 0; x < (int)bitmap.width; x++)
            {
                destination[2 * (y * bitmap.width + x) + 0] = 255;
                destination[2 * (y * bitmap.width + x) + 1] = pixels[x];
            }
            pixels += bitmap.pitch;
        }
    }
    else
    {
        delete glyphData;
        FT_Done_Glyph(ftGlyph);

        throw love::Exception("Unknown TrueType glyph pixel mode: %u", bitmap.pixel_mode);
    }

    FT_Done_Glyph(ftGlyph);

    return glyphData;
}

int TrueTypeRasterizer::GetGlyphCount() const
{
    return face->num_glyphs;
}

bool TrueTypeRasterizer::HasGlyphs(const std::string_view& text) const
{
    if (text.size() == 0)
        return false;

    try
    {
        Utf8Iterator start(text.begin(), text.begin(), text.end());
        Utf8Iterator end(text.end(), text.begin(), text.end());

        while (start != end)
        {
            uint32_t codepoint = *start++;

            if (!this->HasGlyph(codepoint))
                return false;
        }
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return true;
}

bool TrueTypeRasterizer::HasGlyph(uint32_t glyph) const
{
    return FT_Get_Char_Index(this->face, glyph) != 0;
}

float TrueTypeRasterizer::GetKerning(uint32_t left, uint32_t right) const
{
    FT_Vector kerning {};

    const auto leftIndex  = FT_Get_Char_Index(this->face, left);
    const auto rightIndex = FT_Get_Char_Index(this->face, right);

    FT_Get_Kerning(this->face, leftIndex, rightIndex, FT_KERNING_DEFAULT, &kerning);

    return (float)(kerning.x >> 6);
}

bool TrueTypeRasterizer::Accepts(FT_Library library, Data* data)
{
    const auto* faceData = (const FT_Byte*)data->GetData();
    const auto faceSize  = (FT_Long)data->GetSize();

    // Pasing in -1 for the face index lets us test if the data is valid.
    return FT_New_Memory_Face(library, faceData, faceSize, -1, nullptr) == 0;
}
