#include "modules/font/BCFNTRasterizer.hpp"
#include "modules/font/GenericShaper.hpp"

#include "driver/display/citro3d.hpp"

#include <cassert>
#include <cmath>

namespace love
{
    Type BCFNTRasterizer::type("BCFNTRasterizer", &Rasterizer::type);

    static float scaleMetric(uint8_t metric, float scale)
    {
        return metric * scale;
    }

    bool BCFNTRasterizer::accepts(Data* data)
    {
        return (!std::memcmp(data->getData(), "CFNT", 4) || !std::memcmp(data->getData(), "CFNU", 4));
    }

    static int fontGlyphIndexFromCodePoint(CFNT_s* font, uint32_t codepoint)
    {
        int result = 0xFFFF;

        if (codepoint < 0x10000)
        {
            for (CMAP_s* cmap = font->finf.cmap; cmap; cmap = cmap->next)
            {
                if (codepoint < cmap->codeBegin || codepoint > cmap->codeEnd)
                    continue;

                if (cmap->mappingMethod == CMAP_TYPE_DIRECT)
                {
                    result = cmap->indexOffset + (codepoint - cmap->codeBegin);
                    break;
                }

                if (cmap->mappingMethod == CMAP_TYPE_TABLE)
                {
                    result = cmap->indexTable[codepoint - cmap->codeBegin];
                    break;
                }

                int l = -1;
                int r = cmap->nScanEntries;

                while (r - l > 1)
                {
                    int middle = l + (r - l) / 2;
                    if (cmap->scanEntries[middle].code > codepoint)
                        r = middle;
                    else
                        l = middle;
                }

                if (l >= 0 && cmap->scanEntries[l].code == codepoint)
                {
                    result = cmap->scanEntries[l].glyphIndex;
                    break;
                }
            }
        }

        if (result == 0xFFFF) // Bogus CMAP entry. Probably exist to save space by using TABLE mappings?
        {
            if (font->finf.alterCharIndex == 0xFFFF)
                return -1;
            else
                return font->finf.alterCharIndex;
        }

        return result;
    }

    BCFNTRasterizer::BCFNTRasterizer(Data* data, int size)
    {
        this->dpiScale = 1.0f;
        this->size     = std::floor(size * this->dpiScale + 0.5f);

        if (this->size == 0)
            throw love::Exception("Invalid font size: {:d}", this->size);

        /* if we already have this data loaded, fixing this (again) is a bad time™ */
        if ((uintptr_t)fontGetInfo((CFNT_s*)data->getData())->tglp < (uintptr_t)data->getData())
            fontFixPointers((CFNT_s*)data->getData());

        auto* fontInfo  = fontGetInfo((CFNT_s*)data->getData());
        auto* sheetInfo = fontInfo->tglp;

        this->scale = std::floor(this->size * this->dpiScale + 0.5f) / sheetInfo->cellHeight;

        this->metrics.advance = scaleMetric(sheetInfo->maxCharWidth, this->scale);
        this->metrics.ascent  = scaleMetric(fontInfo->ascent, this->scale);
        this->metrics.descent = scaleMetric((fontInfo->height - fontInfo->ascent), this->scale);
        this->metrics.height  = scaleMetric(sheetInfo->cellHeight, this->scale);

        /* todo: more accuracy? */
        for (auto map = fontInfo->cmap; map != nullptr; map = map->next)
            this->glyphCount += (map->codeEnd - map->codeBegin) + 1;

        this->data = data;
    }

    BCFNTRasterizer::~BCFNTRasterizer()
    {}

    TextShaper* BCFNTRasterizer::newTextShaper()
    {
        return new GenericShaper(this);
    }

    bool BCFNTRasterizer::hasGlyph(uint32_t codepoint) const
    {
        const int index  = this->getGlyphIndex(codepoint);
        const auto* info = fontGetInfo((CFNT_s*)this->data->getData());

        return index != info->alterCharIndex && codepoint != '\t';
    }

    int BCFNTRasterizer::getGlyphSpacing(uint32_t codepoint) const
    {
        const int index = this->getGlyphIndex(codepoint);
        const auto flag = GLYPH_POS_CALC_VTXCOORD;

        fontGlyphPos_s result {};
        fontCalcGlyphPos(&result, (CFNT_s*)this->data->getData(), index, flag, this->scale, this->scale);

        return result.xAdvance;
    }

    GlyphData* BCFNTRasterizer::getGlyphDataForIndex(int index) const
    {
        fontGlyphPos_s result {};
        const auto flag = GLYPH_POS_CALC_VTXCOORD;

        fontCalcGlyphPos(&result, (CFNT_s*)this->data->getData(), index, flag, this->scale, this->scale);

        GlyphMetrics metrics {};
        metrics.height   = this->metrics.height;
        metrics.width    = result.width;
        metrics.advance  = result.xAdvance;
        metrics.bearingX = result.xOffset;
        metrics.bearingY = this->metrics.ascent;

        GlyphSheet sheet {};
        sheet.index  = result.sheetIndex;
        sheet.top    = result.texcoord.top;
        sheet.left   = result.texcoord.left;
        sheet.right  = result.texcoord.right;
        sheet.bottom = result.texcoord.bottom;

        const auto* info = fontGetGlyphInfo((CFNT_s*)this->data->getData());

        PixelFormat format;
        if (!citro3d::getConstant((GPU_TEXCOLOR)info->sheetFmt, format))
            throw love::Exception("Invalid texture format: {:u}", info->sheetFmt);

        return new GlyphData(0, metrics, format, sheet);
    }

    int BCFNTRasterizer::getGlyphIndex(uint32_t codepoint) const
    {
        return love::fontGlyphIndexFromCodePoint((CFNT_s*)this->data->getData(), codepoint);
    }

    int BCFNTRasterizer::getGlyphCount() const
    {
        return this->glyphCount;
    }

    ptrdiff_t BCFNTRasterizer::getHandle() const
    {
        return (ptrdiff_t)this->data->getData();
    }
} // namespace love
