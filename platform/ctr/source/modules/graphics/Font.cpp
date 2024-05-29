#include "modules/graphics/Font.hpp"
#include "modules/graphics/Graphics.tcc"

#include "utility/logfile.hpp"

namespace love
{
    Font::Font(Rasterizer* rasterizer, const SamplerState& samplerState) : FontBase(rasterizer, samplerState)
    {
        this->loadVolatile();
    }

    bool Font::loadVolatile()
    {
        this->textureCacheID++;
        this->glyphs.clear();
        this->textures.clear();
        this->createTexture();

        return true;
    }

    void Font::createTexture()
    {
        CFNT_s* font     = (CFNT_s*)this->shaper->getRasterizers()[0]->getHandle();
        const auto* info = fontGetGlyphInfo(font);

        this->textures.reserve(info->nSheets);

        auto graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);

        for (size_t index = 0; index < info->nSheets; index++)
        {
            TextureBase::Settings settings {};
            settings.format = this->pixelFormat;
            settings.width  = info->sheetWidth;
            settings.height = info->sheetHeight;

            auto* texture = graphics->newTexture(settings, nullptr);
            auto* data    = fontGetGlyphSheetTex(font, index);

            Rect rect { 0, 0, settings.width, settings.height };

            texture->replacePixels(data, info->sheetSize, 0, 0, rect, false);
            texture->setSamplerState(this->samplerState);

            this->textures.emplace_back(texture, Acquire::NO_RETAIN);
        }
    }

    const Font::Glyph& Font::addGlyph(TextShaper::GlyphIndex glyphIndex)
    {
        float glyphDPIScale = this->getDPIScale();
        StrongRef<GlyphData> gd(this->getRasterizerGlyphData(glyphIndex, glyphDPIScale), Acquire::NO_RETAIN);

        int width  = gd->getWidth();
        int height = gd->getHeight();

        Glyph glyph {};
        glyph.texture = nullptr;

        std::fill_n(glyph.vertices, 4, GlyphVertex {});

        if (width > 0 && height > 0)
        {
            auto sheet = gd->getSheet();

            TextureBase* texture = this->textures[sheet.index];
            glyph.texture        = texture;

            Color color(1.0f, 1.0f, 1.0f, 1.0f);

            // clang-format off
            std::array<GlyphVertex, 0x04> vertices =
            {
                Vertex { 0.0f, 0.0f,    sheet.left,  sheet.top,    color },
                Vertex { 0.0f, height,  sheet.left,  sheet.bottom, color },
                Vertex { width, 0.0f,   sheet.right, sheet.top,    color },
                Vertex { width, height, sheet.right, sheet.bottom, color }
            };
            // clang-format on

            for (int index = 0; index < 4; index++)
            {
                glyph.vertices[index] = vertices[index];
                glyph.vertices[index].x += gd->getBearingX() / dpiScale;
                glyph.vertices[index].y += gd->getBearingY() / dpiScale;
            }
        }

        const auto index    = packGlyphIndex(glyphIndex);
        this->glyphs[index] = glyph;

        return this->glyphs[index];
    }
} // namespace love
