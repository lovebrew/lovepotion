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

    void FontBase::createTexture()
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
                                                                  Range range, const Color& constantcolor,
                                                                  std::vector<GlyphVertex>& vertices,
                                                                  float extra_spacing, Vector2 offset,
                                                                  TextShaper::TextInfo* info)
    {
        std::vector<TextShaper::GlyphPosition> glyphpositions;
        std::vector<IndexedColor> colors;
        this->shaper->computeGlyphPositions(codepoints, range, offset, extra_spacing, &glyphpositions,
                                            &colors, info);

        size_t vertexStartSize = vertices.size();
        vertices.reserve(vertexStartSize + glyphpositions.size() * 4);

        Color linearConstantColor = gammaCorrectColor(constantcolor);
        Color currentColor        = constantcolor;

        int curcolori = 0;
        int ncolors   = (int)colors.size();

        // Keeps track of when we need to switch textures in our vertex array.
        std::vector<DrawCommand> commands {};

        for (int i = 0; i < (int)glyphpositions.size(); i++)
        {
            const auto& info = glyphpositions[i];

            uint32_t cacheid = textureCacheID;

            const Glyph& glyph = findGlyph(info.glyphIndex);

            // If findGlyph invalidates the texture cache, restart the loop.
            if (cacheid != textureCacheID)
            {
                i = -1; // The next iteration will increment this to 0.
                commands.clear();
                vertices.resize(vertexStartSize);
                curcolori    = 0;
                currentColor = constantcolor;
                continue;
            }

            if (curcolori < ncolors && colors[curcolori].index == i)
            {
                Color c = colors[curcolori].color;

                c.r = std::min(std::max(c.r, 0.0f), 1.0f);
                c.g = std::min(std::max(c.g, 0.0f), 1.0f);
                c.b = std::min(std::max(c.b, 0.0f), 1.0f);
                c.a = std::min(std::max(c.a, 0.0f), 1.0f);

                gammaCorrectColor(c);
                c *= linearConstantColor;
                unGammaCorrectColor(c);

                currentColor = c;
                curcolori++;
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

            auto data               = graphics->requestBatchedDraw(command);
            GlyphVertex* vertexdata = (GlyphVertex*)data.stream;

            std::copy_n(vertexdata, cmd.vertexCount, &vertices[cmd.startVertex]);
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
} // namespace love
