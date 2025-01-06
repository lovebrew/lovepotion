#include "modules/font/freetype/TrueTypeRasterizer.hpp"

#include "modules/font/GenericShaper.hpp"

#include <cmath>

#define E_TTF_LOAD_FAILED "TrueType Font loading error: FT_New_Face failed: {:s} (problem with font file?)"
#define E_TTF_SIZE_FAILED "TrueType Font loading error: FT_Set_Pixel_Sizes failed: {:s} (invalid size?)"

#define E_GLYPH_LOAD_FAILED "TrueType Font glyph error: FT_Load_Glyph failed: {:s}"
#define E_GLYPH_GET_FAILED  "TrueType Font glyph error: FT_Get_Glyph failed: {:s}"

#define E_GLYPH_RENDER_FAILED "TrueType Font glyph error: FT_Glyph_To_Bitmap failed: {:s}"

namespace love
{
    TrueTypeRasterizer::TrueTypeRasterizer(FT_Library library, Data* data, int size, const Settings& settings,
                                           float defaultDPIScale) :
        hinting(settings.hinting)
    {
        this->data.set(data);
        this->dpiScale = settings.dpiScale.get(defaultDPIScale);

        this->size = std::floor(size * dpiScale + 0.5f);
        this->sdf  = settings.sdf;

        if (size <= 0)
            throw love::Exception("Invalid TrueType font size: %d", size);

        FT_Error err = FT_Err_Ok;
        err          = FT_New_Memory_Face(library, (const FT_Byte*)data->getData(), /* first byte in memory */
                                          data->getSize(),                          /* size in bytes        */
                                          0,                                        /* face_index           */
                                          &face);

        if (err != FT_Err_Ok)
            throw love::Exception(E_TTF_LOAD_FAILED, FT_Error_String(err));

        err = FT_Set_Char_Size(face, size << 6, size << 6, 72 * dpiScale, 72 * dpiScale);

        if (err != FT_Err_Ok)
            throw love::Exception(E_TTF_SIZE_FAILED, FT_Error_String(err));

        // Set global metrics
        FT_Size_Metrics& s = face->size->metrics;
        metrics.advance    = (int)(s.max_advance >> 6);
        metrics.ascent     = (int)(s.ascender >> 6);
        metrics.descent    = (int)(s.descender >> 6);
        metrics.height     = (int)(s.height >> 6);
    }

    TrueTypeRasterizer::~TrueTypeRasterizer()
    {
        FT_Done_Face(this->face);
    }

    int TrueTypeRasterizer::getLineHeight() const
    {
        return (int)(this->getHeight() * 1.25);
    }

    FT_UInt TrueTypeRasterizer::hintingToLoadOption(Hinting hint)
    {
        switch (hint)
        {
            default:
            case HINTING_NORMAL:
                return FT_LOAD_DEFAULT;
            case HINTING_LIGHT:
                return FT_LOAD_TARGET_LIGHT;
            case HINTING_MONO:
                return FT_LOAD_TARGET_MONO;
            case HINTING_NONE:
                return FT_LOAD_NO_HINTING;
        }
    }

    int TrueTypeRasterizer::getGlyphSpacing(uint32_t glyph) const
    {
        FT_Glyph ftGlyph;
        FT_Error error     = FT_Err_Ok;
        FT_UInt loadOption = hintingToLoadOption(hinting);

        const auto index = FT_Get_Char_Index(this->face, glyph);
        error            = FT_Load_Glyph(this->face, index, FT_LOAD_DEFAULT | loadOption);

        if (error != FT_Err_Ok)
            return 0;

        error = FT_Get_Glyph(this->face->glyph, &ftGlyph);

        if (error != FT_Err_Ok)
            return 0;

        return (int)(ftGlyph->advance.x >> 16);
    }

    int TrueTypeRasterizer::getGlyphIndex(uint32_t glyph) const
    {
        return FT_Get_Char_Index(this->face, glyph);
    }

    GlyphData* TrueTypeRasterizer::getGlyphDataForIndex(int index) const
    {
        GlyphMetrics metrics = {};
        FT_Glyph ftGlyph;

        FT_Error error     = FT_Err_Ok;
        FT_UInt loadOption = hintingToLoadOption(hinting);

        error = FT_Load_Glyph(this->face, index, FT_LOAD_DEFAULT | loadOption);

        if (error != FT_Err_Ok)
            throw love::Exception(E_GLYPH_LOAD_FAILED, FT_Error_String(error));

        error = FT_Get_Glyph(this->face->glyph, &ftGlyph);

        if (error != FT_Err_Ok)
            throw love::Exception(E_GLYPH_GET_FAILED, FT_Error_String(error));

        FT_Render_Mode mode = FT_RENDER_MODE_NORMAL;
        if (this->sdf)
            mode = FT_RENDER_MODE_SDF;
        else if (this->hinting == HINTING_MONO)
            mode = FT_RENDER_MODE_MONO;

        error = FT_Glyph_To_Bitmap(&ftGlyph, mode, 0, 1);

        if (error != FT_Err_Ok)
        {
            if (mode == FT_RENDER_MODE_SDF)
            {
                error = FT_Glyph_To_Bitmap(&ftGlyph, FT_RENDER_MODE_NORMAL, 0, 1);

                if (error != FT_Err_Ok)
                    throw love::Exception(E_GLYPH_RENDER_FAILED, FT_Error_String(error));
            }
            else
                throw love::Exception(E_GLYPH_RENDER_FAILED, FT_Error_String(error));
        }

        FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)ftGlyph;
        const auto& bitmap         = bitmapGlyph->bitmap;

        metrics.bearingX = bitmapGlyph->left;
        metrics.bearingY = bitmapGlyph->top;
        metrics.width    = bitmap.width;
        metrics.height   = bitmap.rows;
        metrics.advance  = (int)(bitmapGlyph->root.advance.x >> 16);

        GlyphData* data = new GlyphData(0, metrics, PIXELFORMAT_LA8_UNORM);

        const uint8_t* pixels = bitmap.buffer;
        uint8_t* destination  = (uint8_t*)data->getData();

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

                pixels += bitmap.pitch;
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
            delete data;
            FT_Done_Glyph(ftGlyph);
            throw love::Exception("Unknown TrueType glyph pixel mode.");
        }

        FT_Done_Glyph(ftGlyph);
        return data;
    }

    int TrueTypeRasterizer::getGlyphCount() const
    {
        return face->num_glyphs;
    }

    bool TrueTypeRasterizer::hasGlyph(uint32_t glyph) const
    {
        return FT_Get_Char_Index(this->face, glyph) != 0;
    }

    float TrueTypeRasterizer::getKerning(uint32_t left, uint32_t right) const
    {
        FT_Vector kerning;

        const auto leftIndex  = FT_Get_Char_Index(face, left);
        const auto rightIndex = FT_Get_Char_Index(face, right);

        FT_Get_Kerning(face, leftIndex, rightIndex, FT_KERNING_DEFAULT, &kerning);

        return (float)(kerning.x >> 6);
    }

    Rasterizer::DataType TrueTypeRasterizer::getDataType() const
    {
        return Rasterizer::DataType::DATA_TRUETYPE;
    }

    TextShaper* TrueTypeRasterizer::newTextShaper()
    {
        return new GenericShaper(this);
    }

    bool TrueTypeRasterizer::accepts(FT_Library library, Data* data)
    {
        const FT_Byte* base = (const FT_Byte*)data->getData();
        FT_Long size        = (FT_Long)data->getSize();

        return FT_New_Memory_Face(library, base, size, -1, nullptr) == 0;
    }
} // namespace love
