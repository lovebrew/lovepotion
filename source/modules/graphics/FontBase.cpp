#include "modules/graphics/Font.tcc"
#include "modules/graphics/Graphics.tcc"

#include "modules/font/GlyphData.hpp"

#include "common/Console.hpp"
#include "common/Matrix.hpp"
#include "common/math.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace love
{
    int FontBase::fontCount = 0;

    FontBase::FontBase(Rasterizer* rasterizer, const SamplerState& samplerState) :
        shaper(rasterizer->newTextShaper(), Acquire::NO_RETAIN),
        textureWidth(128),
        textureHeight(128),
        samplerState(),
        dpiScale(rasterizer->getDPIScale()),
        textureCacheID(0)
    {
        this->samplerState.minFilter     = samplerState.minFilter;
        this->samplerState.magFilter     = samplerState.magFilter;
        this->samplerState.maxAnisotropy = samplerState.maxAnisotropy;

        if constexpr (!Console::is(Console::CTR))
        {
            while (true)
            {
                if ((shaper->getHeight() * 0.8) * shaper->getHeight() * 30 <= textureWidth * textureHeight)
                    break;

                TextureSize nextsize = getNextTextureSize();

                if (nextsize.width <= textureWidth && nextsize.height <= textureHeight)
                    break;

                textureWidth  = nextsize.width;
                textureHeight = nextsize.height;
            }
        }

        auto* glyphData   = rasterizer->getGlyphData(32);
        this->pixelFormat = glyphData->getFormat();
        glyphData->release();

        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);

        auto supported = graphics->isPixelFormatSupported(this->pixelFormat, PIXELFORMATUSAGEFLAGS_SAMPLE);
        if (this->pixelFormat == PIXELFORMAT_LA8_UNORM && !supported)
            this->pixelFormat = PIXELFORMAT_RGBA8_UNORM;

        ++fontCount;
    }

    FontBase::~FontBase()
    {
        --fontCount;
    }

    FontBase::TextureSize FontBase::getNextTextureSize() const
    {
        TextureSize size = { textureWidth, textureHeight };

        int maxsize = 2048;
        auto gfx    = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);

        if (gfx != nullptr)
        {
            const auto& caps = gfx->getCapabilities();
            maxsize          = (int)caps.limits[GraphicsBase::LIMIT_TEXTURE_SIZE];
        }

        int maxwidth  = std::min(8192, maxsize);
        int maxheight = std::min(4096, maxsize);

        if (size.width * 2 <= maxwidth || size.height * 2 <= maxheight)
        {
            // {128, 128} -> {256, 128} -> {256, 256} -> {512, 256} -> etc.
            if (size.width == size.height)
                size.width *= 2;
            else
                size.height *= 2;
        }

        return size;
    }

    void FontBase::unloadVolatile()
    {
        this->glyphs.clear();
        this->textures.clear();
    }

    GlyphData* FontBase::getRasterizerGlyphData(TextShaper::GlyphIndex glyphindex, float& dpiscale)
    {
        const auto& rasterizer = shaper->getRasterizers()[glyphindex.rasterizerIndex];
        dpiScale               = rasterizer->getDPIScale();

        return rasterizer->getGlyphDataForIndex(glyphindex.index);
    }

    const FontBase::Glyph& FontBase::findGlyph(TextShaper::GlyphIndex glyphindex)
    {
        uint64_t packed = packGlyphIndex(glyphindex);
        const auto it   = glyphs.find(packed);

        if (it != glyphs.end())
            return it->second;

        return this->addGlyph(glyphindex);
    }

    const FontBase::Glyph& FontBase::addGlyph(TextShaper::GlyphIndex glyphIndex)
    {
        float dpiScale = this->getDPIScale();
        StrongRef<GlyphData> gd(this->getRasterizerGlyphData(glyphIndex, dpiScale), Acquire::NO_RETAIN);

        int width  = gd->getWidth();
        int height = gd->getHeight();

        if (width + TEXTURE_PADDING * 2 < textureWidth && height + TEXTURE_PADDING * 2 < textureHeight)
        {
            if (this->textureX + width + TEXTURE_PADDING > textureWidth)
            {
                this->textureX = TEXTURE_PADDING;
                this->textureY += this->rowHeight;
                this->rowHeight = TEXTURE_PADDING;
            }

            if (this->textureY + height + TEXTURE_PADDING > textureHeight)
            {
                this->createTexture();
                return this->addGlyph(glyphIndex);
            }
        }

        Glyph glyph {};
        glyph.texture = nullptr;
        std::fill_n(glyph.vertices, 4, GlyphVertex {});

        if (width > 0 && height > 0)
        {
            TextureBase* texture = this->textures.back();
            glyph.texture        = texture;

            Rect rect = { this->textureX, this->textureY, gd->getWidth(), gd->getHeight() };

            if (this->pixelFormat != gd->getFormat())
            {
                const auto supportedFormat =
                    this->pixelFormat == PIXELFORMAT_RGBA8_UNORM && gd->getFormat() == PIXELFORMAT_LA8_UNORM;

                if (!supportedFormat)
                    throw love::Exception(
                        "Cannot upload font glyphs to texture atlas: unexpected format conversion.");

                const uint8_t* source = (const uint8_t*)gd->getData();
                size_t destSize       = getPixelFormatSliceSize(this->pixelFormat, width, height);
                std::vector<uint8_t> destination(destSize, 0);
                uint8_t* destData = destination.data();

                for (int pixel = 0; pixel < width * height; pixel++)
                {
                    destData[pixel * 4 + 0] = source[pixel * 2 + 0];
                    destData[pixel * 4 + 1] = source[pixel * 2 + 0];
                    destData[pixel * 4 + 2] = source[pixel * 2 + 0];
                    destData[pixel * 4 + 3] = source[pixel * 2 + 1];
                }

                texture->replacePixels(destData, destSize, 0, 0, rect, false);
            }
            else
                texture->replacePixels(gd->getData(), gd->getSize(), 0, 0, rect, false);

            double tX = (double)this->textureX, tY = (double)this->textureY;
            double tW = (double)textureWidth, tH = (double)textureHeight;

            Color color(1.0f, 1.0f, 1.0f, 1.0f);
            int offset = 1;

            // clang-format off
            const GlyphVertex vertices[4] =
            {
                { float(-offset), float(-offset),  (tX - offset) / tW,         (tY - offset) / tH,          color },
                { float(-offset), height + offset, (tX - offset) / tW,         (tY + height + offset) / tH, color },
                { width + offset, float(-offset),  (tX + width + offset) / tW, (tY - offset) / tH,          color },
                { width + offset, height + offset, (tX + width + offset) / tW, (tY + height + offset) / tH, color }
            };
            // clang-format on

            for (int i = 0; i < 4; i++)
            {
                glyph.vertices[i] = vertices[i];
                glyph.vertices[i].x += gd->getBearingX();
                glyph.vertices[i].y -= gd->getBearingY();
            }

            this->textureX += width + TEXTURE_PADDING;
            this->rowHeight = std::max(this->rowHeight, height + TEXTURE_PADDING);
        }

        uint64_t packed      = packGlyphIndex(glyphIndex);
        this->glyphs[packed] = glyph;

        return this->glyphs[packed];
    }

    float FontBase::getKerning(uint32_t leftglyph, uint32_t rightglyph)
    {
        return this->shaper->getKerning(leftglyph, rightglyph);
    }

    float FontBase::getKerning(const std::string& leftchar, const std::string& rightchar)
    {
        return this->shaper->getKerning(leftchar, rightchar);
    }

    float FontBase::getHeight() const
    {
        return this->shaper->getHeight();
    }

    void FontBase::print(GraphicsBase* graphics, const std::vector<ColoredString>& text,
                         const Matrix4& matrix, const Color& constantcolor)
    {
        ColoredCodepoints codepoints;
        getCodepointsFromString(text, codepoints);

        std::vector<GlyphVertex> vertices {};
        auto drawcommands = this->generateVertices(codepoints, Range(), constantcolor, vertices);

        this->printv(graphics, matrix, drawcommands, vertices);
    }

    void FontBase::printf(GraphicsBase* graphics, const std::vector<ColoredString>& text, float wrap,
                          AlignMode align, const Matrix4& matrix, const Color& constantcolor)
    {
        ColoredCodepoints codepoints {};
        getCodepointsFromString(text, codepoints);

        std::vector<GlyphVertex> vertices;
        auto drawcommands = this->generateVerticesFormatted(codepoints, constantcolor, wrap, align, vertices);

        this->printv(graphics, matrix, drawcommands, vertices);
    }

    static bool sortGlyphs(const FontBase::DrawCommand& left, const FontBase::DrawCommand& right)
    {
        if constexpr (Console::is(Console::CTR))
        {
            const auto result = left.texture - right.texture;
            return result < 0;
        }

        return left.texture < right.texture;
    }

    std::vector<FontBase::DrawCommand> FontBase::generateVertices(const ColoredCodepoints& codepoints,
                                                                  Range range, const Color& constantColor,
                                                                  std::vector<GlyphVertex>& vertices,
                                                                  float extra_spacing, Vector2 offset,
                                                                  TextShaper::TextInfo* info)
    {
        std::vector<TextShaper::GlyphPosition> glyphPositions {};
        std::vector<IndexedColor> colors;
        this->shaper->computeGlyphPositions(codepoints, range, offset, extra_spacing, &glyphPositions,
                                            &colors, info);

        size_t vertexStartSize = vertices.size();
        vertices.reserve(vertexStartSize + glyphPositions.size() * 4);

        Color linearConstantColor = gammaCorrectColor(constantColor);
        Color currentColor        = constantColor;

        int currentColorIndex = 0;
        int numColors         = (int)colors.size();

        // Keeps track of when we need to switch textures in our vertex array.
        std::vector<DrawCommand> commands {};

        for (int i = 0; i < (int)glyphPositions.size(); i++)
        {
            const auto& info = glyphPositions[i];

            uint32_t cacheid   = textureCacheID;
            const Glyph& glyph = findGlyph(info.glyphIndex);

            // If findGlyph invalidates the texture cache, restart the loop.
            if (cacheid != textureCacheID)
            {
                i = -1; // The next iteration will increment this to 0.
                commands.clear();
                vertices.resize(vertexStartSize);
                currentColorIndex = 0;
                currentColor      = constantColor;
                continue;
            }

            if (currentColorIndex < numColors && colors[currentColorIndex].index == i)
            {
                Color c = colors[currentColorIndex].color;

                c.r = std::clamp(c.r, 0.0f, 1.0f);
                c.g = std::clamp(c.g, 0.0f, 1.0f);
                c.b = std::clamp(c.b, 0.0f, 1.0f);
                c.a = std::clamp(c.a, 0.0f, 1.0f);

                gammaCorrectColor(c);
                c *= linearConstantColor;
                unGammaCorrectColor(c);

                currentColor = c;
                currentColorIndex++;
            }

            if (glyph.texture != nullptr)
            {
                // Copy the vertices and set their colors and relative positions.
                for (int j = 0; j < 4; j++)
                {
                    vertices.push_back(glyph.vertices[j]);
                    vertices.back().x += info.position.x;
                    vertices.back().y += info.position.y;
                    vertices.back().color = currentColor;
                }

                // Check if glyph texture has changed since the last iteration.
                if (commands.empty() || commands.back().texture != glyph.texture)
                {
                    // Add a new draw command if the texture has changed.
                    DrawCommand cmd;
                    cmd.startVertex = (int)vertices.size() - 4;
                    cmd.vertexCount = 0;
                    cmd.texture     = glyph.texture;
                    commands.push_back(cmd);
                }

                commands.back().vertexCount += 4;
            }
        }

        std::sort(commands.begin(), commands.end(), sortGlyphs);

        return commands;
    }

    std::vector<FontBase::DrawCommand> FontBase::generateVerticesFormatted(const ColoredCodepoints& text,
                                                                           const Color& constantcolor,
                                                                           float wrap, AlignMode align,
                                                                           std::vector<GlyphVertex>& vertices,
                                                                           TextShaper::TextInfo* info)
    {
        wrap = std::max(wrap, 0.0f);

        uint32_t cacheid = textureCacheID;

        std::vector<DrawCommand> drawcommands;
        vertices.reserve(text.codepoints.size() * 4);

        std::vector<Range> ranges;
        std::vector<int> widths;
        shaper->getWrap(text, wrap, ranges, &widths);

        float y        = 0.0f;
        float maxwidth = 0.0f;

        for (int i = 0; i < (int)ranges.size(); i++)
        {
            const auto& range = ranges[i];

            if (!range.isValid())
            {
                y += getHeight() * getLineHeight();
                continue;
            }

            float width = (float)widths[i];
            love::Vector2 offset(0.0f, floorf(y));
            float extraspacing = 0.0f;

            maxwidth = std::max(width, maxwidth);

            switch (align)
            {
                case ALIGN_RIGHT:
                    offset.x = floorf(wrap - width);
                    break;
                case ALIGN_CENTER:
                    offset.x = floorf((wrap - width) / 2.0f);
                    break;
                case ALIGN_JUSTIFY:
                {
                    auto start      = text.codepoints.begin() + range.getOffset();
                    auto end        = start + range.getSize();
                    float numspaces = std::count(start, end, ' ');

                    if (text.codepoints[range.last] == ' ')
                        --numspaces;

                    if (width < wrap && numspaces >= 1)
                        extraspacing = (wrap - width) / (numspaces - 1);
                    else
                        extraspacing = 0.0f;

                    break;
                }
                case ALIGN_LEFT:
                default:
                    break;
            }

            auto newcommands = generateVertices(text, range, constantcolor, vertices, extraspacing, offset);

            if (!newcommands.empty())
            {
                auto firstcmd = newcommands.begin();

                // If the first draw command in the new list has the same texture
                // as the last one in the existing list we're building and its
                // vertices are in-order, we can combine them (saving a draw call.)
                if (!drawcommands.empty())
                {
                    auto prevcmd = drawcommands.back();
                    if (prevcmd.texture == firstcmd->texture &&
                        (prevcmd.startVertex + prevcmd.vertexCount) == firstcmd->startVertex)
                    {
                        drawcommands.back().vertexCount += firstcmd->vertexCount;
                        ++firstcmd;
                    }
                }

                // Append the new draw commands to the list we're building.
                drawcommands.insert(drawcommands.end(), firstcmd, newcommands.end());
            }

            y += getHeight() * getLineHeight();
        }

        if (info != nullptr)
        {
            info->width  = (int)maxwidth;
            info->height = (int)y;
        }

        if (cacheid != textureCacheID)
        {
            vertices.clear();
            drawcommands = generateVerticesFormatted(text, constantcolor, wrap, align, vertices);
        }

        return drawcommands;
    }

    static constexpr auto shaderType =
        (Console::is(Console::CTR)) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_TEXTURE;

    void FontBase::printv(GraphicsBase* graphics, const Matrix4& matrix,
                          const std::vector<DrawCommand>& drawcommands,
                          const std::vector<GlyphVertex>& vertices)
    {
        if (vertices.empty() || drawcommands.empty())
            return;

        Matrix4 m(graphics->getTransform(), matrix);

        for (const DrawCommand& cmd : drawcommands)
        {
            BatchedDrawCommand command {};
            command.format      = CommonFormat::XYf_STf_RGBAf;
            command.indexMode   = TRIANGLEINDEX_QUADS;
            command.vertexCount = cmd.vertexCount;
            command.texture     = cmd.texture;
            command.isFont      = true;
            command.shaderType  = shaderType;

            auto data               = graphics->requestBatchedDraw(command);
            GlyphVertex* vertexdata = (GlyphVertex*)data.stream;

            std::copy_n(&vertices[cmd.startVertex], cmd.vertexCount, vertexdata);
            m.transformXY(vertexdata, &vertices[cmd.startVertex], cmd.vertexCount);
        }
    }

    int FontBase::getWidth(const std::string& str)
    {
        return this->shaper->getWidth(str);
    }

    int FontBase::getWidth(uint32_t glyph)
    {
        return this->shaper->getGlyphAdvance(glyph);
    }

    void FontBase::getWrap(const ColoredCodepoints& codepoints, float wraplimit, std::vector<Range>& ranges,
                           std::vector<int>* linewidths)
    {
        this->shaper->getWrap(codepoints, wraplimit, ranges, linewidths);
    }

    void FontBase::getWrap(const std::vector<ColoredString>& text, float wraplimit,
                           std::vector<std::string>& lines, std::vector<int>* linewidths)
    {
        this->shaper->getWrap(text, wraplimit, lines, linewidths);
    }

    void FontBase::setLineHeight(float height)
    {
        this->shaper->setLineHeight(height);
    }

    float FontBase::getLineHeight() const
    {
        return this->shaper->getLineHeight();
    }

    const SamplerState& FontBase::getSamplerState() const
    {
        return this->samplerState;
    }

    int FontBase::getAscent() const
    {
        return this->shaper->getAscent();
    }

    int FontBase::getDescent() const
    {
        return this->shaper->getDescent();
    }

    int FontBase::getBaseline() const
    {
        return this->shaper->getBaseline();
    }

    bool FontBase::hasGlyph(uint32_t glyph) const
    {
        return this->shaper->hasGlyph(glyph);
    }

    bool FontBase::hasGlyphs(const std::string& text) const
    {
        return this->shaper->hasGlyphs(text);
    }

    float FontBase::getDPIScale() const
    {
        return this->dpiScale;
    }

    uint32_t FontBase::getTextureCacheID() const
    {
        return this->textureCacheID;
    }

    void FontBase::setFallbacks(const std::vector<FontBase*>& fallbacks)
    {
        std::vector<Rasterizer*> rasterizers {};

        for (const FontBase* font : fallbacks)
            rasterizers.push_back(font->shaper->getRasterizers()[0]);

        this->shaper->setFallbacks(rasterizers);
        this->glyphs.clear();

        if constexpr (!Console::is(Console::CTR))
        {
            this->textureCacheID++;

            while (this->textures.size() > 1)
                this->textures.pop_back();
        }
    }

    void FontBase::setSamplerState(const SamplerState& state)
    {
        this->samplerState.minFilter     = state.minFilter;
        this->samplerState.magFilter     = state.magFilter;
        this->samplerState.maxAnisotropy = state.maxAnisotropy;

        for (const auto& texture : this->textures)
            texture->setSamplerState(state);
    }
} // namespace love
