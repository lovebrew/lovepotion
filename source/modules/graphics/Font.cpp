#include "modules/graphics/Font.hpp"
#include "modules/graphics/Graphics.hpp"

namespace love
{
    Font::Font(Rasterizer* rasterizer, const SamplerState& samplerState) : FontBase(rasterizer, samplerState)
    {
        this->loadVolatile();
    }

    void Font::createTexture()
    {
        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
        graphics->flushBatchedDraws();

        TextureBase* texture = nullptr;
        TextureSize size     = { this->textureWidth, this->textureHeight };
        TextureSize nextSize = this->getNextTextureSize();

        bool recreateTexture = false;

        if ((nextSize.width > size.width || nextSize.height > size.height) && !this->textures.empty())
        {
            recreateTexture = true;
            size            = nextSize;
            this->textures.pop_back();
        }

        TextureBase::Settings settings {};
        settings.format = pixelFormat;
        settings.width  = size.width;
        settings.height = size.height;

        texture = graphics->newTexture(settings, nullptr);
        texture->setSamplerState(samplerState);

        {
            auto dataSize   = getPixelFormatSliceSize(pixelFormat, size.width, size.height);
            auto pixelCount = size.width * size.height;

            std::vector<uint8_t> empty(dataSize, 0);

            if (this->shaper->getRasterizers()[0]->getDataType() == Rasterizer::DATA_TRUETYPE)
            {
                if (this->pixelFormat == PIXELFORMAT_LA8_UNORM)
                {
                    for (size_t i = 0; i < pixelCount; i++)
                        empty[i * 2 + 0] = 255;
                }
                else if (this->pixelFormat == PIXELFORMAT_RGBA8_UNORM)
                {
                    for (size_t i = 0; i < pixelCount; i++)
                    {
                        empty[i * 4 + 0] = 255;
                        empty[i * 4 + 1] = 255;
                        empty[i * 4 + 2] = 255;
                    }
                }
            }

            Rect rect = { 0, 0, size.width, size.height };
            texture->replacePixels(empty.data(), empty.size(), 0, 0, rect, false);
        }

        textures.emplace_back(texture);
        this->textureWidth  = size.width;
        this->textureHeight = size.height;

        this->rowHeight = this->textureX = this->textureY = TEXTURE_PADDING;

        if (recreateTexture)
        {
            this->textureCacheID++;
            std::vector<TextShaper::GlyphIndex> glyphsToAdd;

            for (const auto& glyph : this->glyphs)
                glyphsToAdd.push_back(unpackGlyphIndex(glyph.first));

            this->glyphs.clear();

            for (const auto& glyphIndex : glyphsToAdd)
                this->addGlyph(glyphIndex);
        }
    }

    bool Font::loadVolatile()
    {
        this->textureCacheID++;
        this->glyphs.clear();
        this->textures.clear();
        this->createTexture();

        return true;
    }

    const FontBase::Glyph& Font::addGlyph(TextShaper::GlyphIndex glyphIndex)
    {
        return FontBase::addGlyph(glyphIndex);
    }
} // namespace love
