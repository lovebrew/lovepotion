#include "modules/graphics/freetype/Font.hpp"
#include "modules/graphics/Graphics.tcc"

namespace love
{
    Font::Font(Rasterizer* rasterizer, const SamplerState& samplerState) : FontBase(rasterizer, samplerState)
    {
        this->loadVolatile();
    }

    bool Font::loadVolatile()
    {
        textureCacheID++;
        glyphs.clear();
        textures.clear();
        createTexture();
        return true;
    }

    void Font::createTexture()
    {
        auto* module = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
        module->flushBatchedDraws();

        TextureBase* texture = nullptr;
        TextureSize size     = { this->textureWidth, this->textureHeight };
        TextureSize nextSize = this->getNextTextureSize();
        bool recreateTexture = false;

        const bool biggerWidth  = nextSize.width > size.width;
        const bool biggerHeight = nextSize.height > size.height;

        if ((biggerWidth || biggerHeight) && !this->textures.empty())
        {
            recreateTexture = true;
            size            = nextSize;
            this->textures.pop_back();
        }

        TextureBase::Settings settings {};
        settings.format = this->pixelFormat;
        settings.width  = size.width;
        settings.height = size.height;

        texture = module->newTexture(settings, nullptr);
        texture->setSamplerState(this->samplerState);

        {
            size_t dataSize   = getPixelFormatSliceSize(this->pixelFormat, size.width, size.height);
            size_t pixelCount = size.width * size.height;

            std::vector<uint8_t> empty(dataSize, 0);
            if (shaper->getRasterizers()[0]->getDataType() == Rasterizer::DATA_TRUETYPE)
            {
                if (this->pixelFormat == PIXELFORMAT_LA8_UNORM)
                {
                    for (size_t index = 0; index < pixelCount; index++)
                        empty[index * 2 + 0] = 255;
                }
                else if (this->pixelFormat == PIXELFORMAT_RGBA8_UNORM)
                {
                    for (size_t index = 0; index < pixelCount; index++)
                    {
                        empty[index * 4 + 0] = 255;
                        empty[index * 4 + 1] = 255;
                        empty[index * 4 + 2] = 255;
                    }
                }
            }

            Rect rectangle = { 0, 0, size.width, size.height };
            texture->replacePixels(empty.data(), empty.size(), 0, 0, rectangle, false);
        }

        this->textures.emplace_back(texture, Acquire::NO_RETAIN);

        this->textureWidth  = size.width;
        this->textureHeight = size.height;
        this->rowHeight = this->textureX = this->textureY = TEXTURE_PADDING;

        if (recreateTexture)
        {
            this->textureCacheID++;
            std::vector<TextShaper::GlyphIndex> glyphsToAdd;

            for (const auto& glyphPair : this->glyphs)
                glyphsToAdd.push_back(unpackGlyphIndex(glyphPair.first));

            this->glyphs.clear();

            for (auto glyphIndex : glyphsToAdd)
                this->addGlyph(glyphIndex);
        }
    }
} // namespace love
