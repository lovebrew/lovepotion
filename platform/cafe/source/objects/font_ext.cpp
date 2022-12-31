#include <objects/font_ext.hpp>

#include <common/matrix_ext.hpp>

#include <modules/graphics_ext.hpp>

#include <objects/texture_ext.hpp>

#include <utf8.h>

#include <algorithm>
#include <limits>

using namespace love;

Font<Console::CAFE>::Font(Rasterizer<Console::CAFE>* rasterizer, const SamplerState& state) :
    Font<>::Font(),
    textureWidth(128),
    textureHeight(128),
    textureCacheID(0)
{
    this->rasterizers = std::vector<StrongRasterizer>({ rasterizer });
    this->height      = rasterizer->GetHeight();
    this->dpiScale    = rasterizer->GetDPIScale();

    this->samplerState.minFilter     = state.minFilter;
    this->samplerState.magFilter     = state.magFilter;
    this->samplerState.maxAnisotropy = state.maxAnisotropy;

    /* find the best texture size match for the font size */
    while (true)
    {
        if ((this->height * 0.8) * this->height * 30 <= this->textureWidth * this->textureHeight)
            break;

        TextureSize nextSize = this->GetNextTextureSize();

        if (nextSize.width <= this->textureWidth && nextSize.height <= this->textureHeight)
            break;

        this->textureWidth  = nextSize.width;
        this->textureHeight = nextSize.height;
    }

    /* glyph data for the space character */
    auto* glyphData = rasterizer->GetGlyphData(Font::SPACE_GLYPH);
    this->format    = glyphData->GetFormat();
    glyphData->Release();

    auto* graphics = Module::GetInstance<Graphics<Console::CAFE>>(Module::M_GRAPHICS);

    auto usage = PIXELFORMAT_USAGE_FLAGS_SAMPLE;
    if (format == PIXELFORMAT_LA8_UNORM && !graphics->IsPixelFormatSupported(format, usage))
        format = PIXELFORMAT_RGBA8_UNORM;

    /* has no tab character */
    if (!rasterizer->HasGlyph(Font::TAB_GLYPH))
        this->useSpacesAsTab = true;

    this->LoadVolatile();
}

Font<Console::CAFE>::TextureSize Font<Console::CAFE>::GetNextTextureSize() const
{
    TextureSize size = { this->textureWidth, this->textureHeight };

    /* todo: query the actual texture limit from the graphics module */

    int maxWidth  = std::min(8192, MAX_TEXTURE_SIZE);
    int maxHeight = std::min(4096, MAX_TEXTURE_SIZE);

    if (size.width * 2 <= maxWidth || size.height * 2 <= maxHeight)
    {
        if (size.width == size.height)
            size.width *= 2;
        else
            size.height *= 2;
    }

    return size;
}

bool Font<Console::CAFE>::LoadVolatile()
{
    this->textureCacheID++;
    this->glyphs.clear();
    this->textures.clear();
    this->CreateTexture();

    return true;
}

void Font<Console::CAFE>::UnloadVolatile()
{
    this->glyphs.clear();
    this->textures.clear();
}

void Font<Console::CAFE>::CreateTexture()
{
    auto* graphics = Module::GetInstance<Graphics<Console::CAFE>>(Module::M_GRAPHICS);

    Texture<Console::CAFE>* texture = nullptr;

    TextureSize size = { this->textureWidth, this->textureHeight };
    auto nextSize    = this->GetNextTextureSize();

    bool remakeTexture = false;

    /* with an existing texture, replace it with something larger */
    const bool newSizeBigger = (nextSize.width > size.width || nextSize.height > size.height);

    if (newSizeBigger && !this->textures.empty())
    {
        remakeTexture = true;
        size          = nextSize;
        this->textures.pop_back();
    }

    /* make texture settings */
    Texture<>::Settings settings {};
    settings.format = this->format;
    settings.width  = size.width;
    settings.height = size.height;

    /* create the new texture */
    texture = graphics->NewTexture(settings, nullptr);
    texture->SetSamplerState(this->samplerState);

    {
        /* replace the pixels in the Texture accordingly */
        const auto dataSize = love::GetPixelFormatSliceSize(this->format, size.width, size.height);
        const auto pixelCount = size.width * size.height;

        std::vector<uint8_t> emptyData(dataSize, 0);

        if (this->rasterizers[0]->GetDataType() == Rasterizer<>::DATA_TRUETYPE)
        {
            if (this->format == PIXELFORMAT_LA8_UNORM)
            {
                for (int index = 0; index < pixelCount; index++)
                    emptyData[index * 2 + 0] = 255;
            }
            else if (this->format == PIXELFORMAT_RGBA8_UNORM)
            {
                for (int index = 0; index < pixelCount; index++)
                {
                    emptyData[index * 4 + 0] = 255;
                    emptyData[index * 4 + 1] = 255;
                    emptyData[index * 4 + 2] = 255;
                }
            }
        }

        Rect rectangle = { 0, 0, size.width, size.height };
        texture->ReplacePixels(emptyData.data(), emptyData.size(), 0, 0, rectangle, false);
    }

    this->textures.emplace_back(texture, Acquire::NORETAIN);

    this->textureWidth  = size.width;
    this->textureHeight = size.height;

    /* set the row height, x, and y to the padding value */
    this->rowHeight = this->textureX = this->textureY = Font::TEXTURE_PADDING;

    /*
    ** we want to re-add old glyphs
    ** if we recreated the existing Texture object
    */
    if (remakeTexture)
    {
        this->textureCacheID++;
        std::vector<uint32_t> _glyphs;

        for (const auto& pair : this->glyphs)
            _glyphs.push_back(pair.first);

        this->glyphs.clear();

        for (auto glyph : _glyphs)
            this->AddGlyph(glyph);
    }
}

GlyphData* Font<Console::CAFE>::GetRasterizerGlyphData(uint32_t glyph, float& dpiScale)
{
    /* we need to make GlyphData for the space character */
    if (glyph == Font::TAB_GLYPH && this->useSpacesAsTab)
    {
        auto* space = this->rasterizers[0]->GetGlyphData(Font::SPACE_GLYPH);
        auto format = space->GetFormat();

        GlyphData::GlyphMetrics metrics {};
        metrics.advance  = space->GetAdvance() * Font::SPACES_PER_TAB;
        metrics.bearingX = space->GetBearingX();
        metrics.bearingY = space->GetBearingY();

        space->Release();

        dpiScale = this->rasterizers[0]->GetDPIScale();
        return new GlyphData(glyph, metrics, format);
    }

    for (const auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(glyph))
        {
            dpiScale = rasterizer->GetDPIScale();
            return rasterizer->GetGlyphData(glyph);
        }
    }

    dpiScale = this->rasterizers[0]->GetDPIScale();
    return this->rasterizers[0]->GetGlyphData(glyph);
}

const Font<Console::CAFE>::Glyph& Font<Console::CAFE>::AddGlyph(uint32_t glyph)
{
    float dpiScale = this->GetDPIScale();
    StrongReference<GlyphData> data(this->GetRasterizerGlyphData(glyph, dpiScale));

    auto width             = data->GetWidth();
    auto height            = data->GetHeight();
    const auto glyphFormat = data->GetFormat();

    const auto glyphWidthPadding  = width + Font::TEXTURE_PADDING * 2;
    const auto glyphHeightPadding = height + Font::TEXTURE_PADDING * 2;

    if (glyphWidthPadding < this->textureWidth && glyphHeightPadding < this->textureHeight)
    {
        /* if this is somehow bigger than the texture width, make a new row */
        if (this->textureX + width + Font::TEXTURE_PADDING > this->textureWidth)
        {
            this->textureX = Font::TEXTURE_PADDING;
            this->textureY += this->rowHeight;
            this->rowHeight = Font::TEXTURE_PADDING;
        }

        /* we would be entirely out of space if the height went over the texture height */
        if (this->textureY + height + Font::TEXTURE_PADDING > this->textureHeight)
        {
            this->CreateTexture();

            /* add the glyph for the new texture */
            return this->AddGlyph(glyph);
        }
    }

    Glyph _glyph {};
    _glyph.texture = nullptr;
    _glyph.spacing = std::floor(data->GetAdvance() / dpiScale + 0.5f);
    std::fill_n(_glyph.vertices, 4, vertex::Vertex {});

    /* don't waste space on empty glyphs */
    if (width > 0 && height > 0)
    {
        Texture<Console::CAFE>* texture = this->textures.back();
        _glyph.texture                  = texture;

        Rect rectangle = { this->textureX, this->textureY, data->GetWidth(), data->GetHeight() };

        /* if the formats don't match, we need to convert it */
        if (this->format != data->GetFormat())
        {
            if (!(this->format == PIXELFORMAT_RGBA8_UNORM && glyphFormat == PIXELFORMAT_LA8_UNORM))
            {
                throw love::Exception(
                    "Cannot upload font glyphs to texture atlas: unexpected format conversion");
            }

            const auto* source = (const uint8_t*)data->GetData();
            const auto size    = love::GetPixelFormatSliceSize(this->format, width, height);

            std::vector<uint8_t> destination(size, 0);
            auto* destinationData = destination.data();

            for (int pixel = 0; pixel < width * height; pixel++)
            {
                destinationData[pixel * 4 + 0] = source[pixel * 2 + 0];
                destinationData[pixel * 4 + 1] = source[pixel * 2 + 0];
                destinationData[pixel * 4 + 2] = source[pixel * 2 + 0];
                destinationData[pixel * 4 + 3] = source[pixel * 2 + 1];
            }

            texture->ReplacePixels(destinationData, size, 0, 0, rectangle, false);
        }
        else
            texture->ReplacePixels(data->GetData(), data->GetSize(), 0, 0, rectangle, false);

        const auto x = (double)this->textureX;
        const auto y = (double)this->textureY;

        const auto _width  = (double)this->textureWidth;
        const auto _height = (double)this->textureHeight;

        const auto offset = 1.0f;
        const auto color  = Color(Color::WHITE).array();

        // clang-format off
        const vertex::Vertex vertices[4] =
        {
            /* x                                      y                             z                u                                                 v                                                  */
            {{ -offset,                               -offset,                      0.0f }, color, { (x - offset) / _width,         (y - offset) / _height          }},
            {{ -offset,                               (height + offset) / dpiScale, 0.0f }, color, { (x - offset) / _width,         (y + height + offset) / _height }},
            {{ (width + offset) / dpiScale,           (height + offset) / dpiScale, 0.0f }, color, { (x + width + offset) / _width, (y + height + offset) / _height }},
            {{ (width + offset) / dpiScale,           -offset,                      0.0f }, color, { (x + width + offset) / _width, (y - offset) / _height          }}
        };
        // clang-format on

        /* copy the vertex data into the Glyph and set proper bearing */
        for (size_t index = 0; index < 4; index++)
        {
            _glyph.vertices[index] = vertices[index];

            _glyph.vertices[index].position[0] += data->GetBearingX() / dpiScale;
            _glyph.vertices[index].position[1] -= data->GetBearingY() / dpiScale;
        }

        this->textureX += width + Font::TEXTURE_PADDING;
        this->rowHeight = std::max(this->rowHeight, height + Font::TEXTURE_PADDING);
    }

    this->glyphs[glyph] = _glyph;
    return this->glyphs[glyph];
}

const Font<Console::CAFE>::Glyph& Font<Console::CAFE>::FindGlyph(uint32_t glyph)
{
    const auto iterator = this->glyphs.find(glyph);

    if (iterator != this->glyphs.end())
        return iterator->second;

    return this->AddGlyph(glyph);
}

float Font<Console::CAFE>::GetKerning(uint32_t left, uint32_t right)
{
    const auto packed   = ((uint64_t)left << 32) | (uint64_t)right;
    const auto iterator = this->kernings.find(packed);

    if (iterator != this->kernings.end())
        return iterator->second;

    auto kerning = std::floor(this->rasterizers[0]->GetKerning(left, right));

    for (const auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(left) && rasterizer->HasGlyph(right))
        {
            kerning = std::floor(rasterizer->GetKerning(left, right));
            break;
        }
    }

    this->kernings[packed] = kerning / this->dpiScale + 0.5f;
    return this->kernings[packed];
}

float Font<Console::CAFE>::GetKerning(std::string_view leftChar, std::string_view rightChar)
{
    uint32_t left;
    uint32_t right;

    try
    {
        left  = utf8::peek_next(leftChar.begin(), leftChar.end());
        right = utf8::peek_next(rightChar.begin(), rightChar.end());
    }
    catch (utf8::exception& exception)
    {
        throw love::Exception("UTF-8 decoding error: %s", exception.what());
    }

    return this->GetKerning(left, right);
}

void Font<Console::CAFE>::GetCodepointsFromString(std::string_view text, Codepoints& codepoints)
{
    codepoints.reserve(text.size());

    try
    {
        Utf8Iterator start(text.begin(), text.begin(), text.end());
        Utf8Iterator end(text.end(), text.begin(), text.end());

        while (start != end)
        {
            auto glyph = *start++;
            codepoints.push_back(glyph);
        }
    }
    catch (utf8::exception& exception)
    {
        throw love::Exception("UTF-8 decoding error: %s", exception.what());
    }
}

void Font<Console::CAFE>::GetCodepointsFromString(const ColoredStrings& strings,
                                                  ColoredCodepoints& codepoints)
{
    if (strings.empty())
        return;

    codepoints.codepoints.reserve(strings[0].string.size());
    for (const auto& coloredString : strings)
    {
        if (coloredString.string.size() == 0)
            continue;

        IndexedColor color = { coloredString.color, (int)codepoints.codepoints.size() };
        codepoints.colors.push_back(color);

        Font::GetCodepointsFromString(coloredString.string, codepoints.codepoints);
    }

    if (codepoints.colors.size() == 1)
    {
        IndexedColor color = codepoints.colors[0];

        if (color.index == 0 && color.color == Color::WHITE)
            codepoints.colors.pop_back();
    }
}

std::vector<Font<Console::CAFE>::DrawCommand> Font<Console::CAFE>::GenerateVertices(
    const ColoredCodepoints& text, const Color& constantColor,
    std::vector<vertex::Vertex>& vertices, float extraSpacing, Vector2 offset, TextInfo* info)
{
    float dx = offset.x;
    float dy = offset.y;

    float heightOffset = 0.0f;
    if (this->rasterizers[0]->GetDataType() == Rasterizer<>::DATA_TRUETYPE)
        heightOffset = this->GetBaseline();

    int maxWidth = 0;
    std::vector<DrawCommand> commands;

    /* reserve max possible vertex size */
    size_t startSize = vertices.size();
    vertices.reserve(startSize + text.codepoints.size() * 4);

    uint32_t previousGlyph    = 0;
    Color linearConstantColor = Graphics<>::GammaCorrectColor(constantColor);

    Color currentColor = constantColor;

    int colorIndex        = -1;
    const auto colorCount = (int)text.colors.size();

    for (int index = 0; index < (int)text.codepoints.size(); index++)
    {
        /* current glyph to work on */
        const auto glyph = text.codepoints[index];

        /* gamma correct the glyph's color */
        if (colorIndex + 1 < colorCount && text.colors[colorIndex + 1].index == index)
        {
            auto glyphColor = text.colors[++colorIndex].color;

            Graphics<>::GammaCorrectColor(glyphColor);
            glyphColor *= linearConstantColor;
            Graphics<>::UnGammaCorrectColor(glyphColor);
        }

        if (glyph == Font::NEWLINE_GLYPH)
        {
            if (dx > maxWidth)
                maxWidth = dx;

            /* wrap the new line, make sure it doesn't get printed */
            dy += std::floor(this->GetHeight() * this->GetLineHeight() + 0.5f);
            dx            = offset.x;
            previousGlyph = 0;

            continue;
        }

        if (glyph == Font::CARRIAGE_GLYPH)
            continue;

        const auto cacheId    = this->textureCacheID;
        const auto& glyphData = this->FindGlyph(glyph);

        /*
        ** the glyph invalidated our cache
        ** so we will want to restart the loop
        */
        if (cacheId != this->textureCacheID)
        {
            index    = -1;
            maxWidth = 0;

            dx = offset.x;
            dy = offset.y;

            commands.clear();
            vertices.resize(startSize);

            previousGlyph = 0;

            colorIndex   = -1;
            currentColor = constantColor;

            continue;
        }

        dx += this->GetKerning(previousGlyph, glyph);

        if (glyphData.texture != nullptr)
        {
            for (int j = 0; j < 4; j++)
            {
                vertices.push_back(glyphData.vertices[j]);

                vertices.back().position[0] += dx;
                vertices.back().position[1] += dy + heightOffset;
                vertices.back().color = currentColor.array();
            }

            if (commands.empty() || commands.back().texture != glyphData.texture)
            {
                DrawCommand command {};
                command.start   = (int)vertices.size() - 4;
                command.count   = 0;
                command.texture = glyphData.texture;

                commands.push_back(command);
            }

            commands.back().count += 4;
        }

        /* advance the x position */
        dx += glyphData.spacing;

        if (glyph == Font::SPACE_GLYPH && extraSpacing != 0.0f)
            dx = std::floor(dx + extraSpacing);

        previousGlyph = glyph;
    }

    /* texture binds are expensive, so we should sort by that first */
    const auto drawsort = [](const DrawCommand& a, const DrawCommand& b) -> bool {
        if (a.texture != b.texture)
            return a.texture < b.texture;
        else
            return a.start < b.start;
    };

    std::sort(commands.begin(), commands.end(), drawsort);

    if (dx > maxWidth)
        maxWidth = (int)dx;

    if (info != nullptr)
    {
        info->width = maxWidth - offset.x;

        const auto height = this->GetHeight() * this->GetLineHeight() + 0.5f;
        info->height      = (int)dy + (dx > 0.0f ? std::floor(height) : 0) - offset.y;
    }

    return commands;
}

std::vector<Font<Console::CAFE>::DrawCommand> Font<Console::CAFE>::GenerateVerticesFormatted(
    const ColoredCodepoints& text, const Color& constantColor, float wrap, AlignMode align,
    std::vector<vertex::Vertex>& vertices, TextInfo* info)
{
    wrap             = std::max(wrap, 0.0f);
    uint32_t cacheId = this->textureCacheID;

    std::vector<DrawCommand> commands;
    vertices.reserve(text.codepoints.size() * 4);

    /* todo: finish */

    return commands;
}

void Font<Console::CAFE>::Printv(Graphics<Console::CAFE>& graphics,
                                 const Matrix4<Console::CAFE>& transform,
                                 const std::vector<DrawCommand>& commands,
                                 const std::vector<vertex::Vertex>& vertices)
{
    if (vertices.empty() || commands.empty())
        return;

    Matrix4<Console::CAFE> matrix(graphics.GetTransform(), transform);

    for (const auto& command : commands)
    {
        // auto drawCommand         = Graphics<Console::CAFE>::DrawCommand(command.count);
        // drawCommand.shader       = Shader<>::STANDARD_TEXTURE;
        // drawCommand.format       = vertex::CommonFormat::TEXTURE;
        // drawCommand.primitveType = vertex::PRIMITIVE_QUADS;
        // drawCommand.handles      = { command.texture->GetHandle() };

        // LOG("Transforming verts");
        // matrix.TransformXYVert(drawCommand.Positions().get(), &vertices[command.start],
        //                        command.count);

        // LOG("Filling verts");
        // drawCommand.FillVertices(vertices.data());

        // LOG("Rendering..");
        // Renderer<Console::CAFE>::Instance().Render(drawCommand);
    }
}

void Font<Console::CAFE>::Print(Graphics<Console::CAFE>& graphics, const ColoredStrings& text,
                                const Matrix4<Console::CAFE>& matrix, const Color& color)
{
    ColoredCodepoints codepoints {};
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<vertex::Vertex> vertices;
    std::vector<DrawCommand> commands = this->GenerateVertices(codepoints, color, vertices);

    this->Printv(graphics, matrix, commands, vertices);
}

void Font<Console::CAFE>::Printf(Graphics<Console::CAFE>& graphics, const ColoredStrings& text,
                                 float wrap, AlignMode alignment,
                                 const Matrix4<Console::CAFE>& matrix, const Color& color)
{
    ColoredCodepoints codepoints {};
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<vertex::Vertex> vertices;
    std::vector<DrawCommand> commands =
        this->GenerateVerticesFormatted(codepoints, color, wrap, alignment, vertices);

    // printv(gfx, m, drawcommands, vertices);
}

void Font<Console::CAFE>::SetSamplerState(const SamplerState& state)
{
    for (const auto& texture : this->textures)
        texture->SetSamplerState(state);
}

/* todo */
int Font<Console::CAFE>::GetWidth(std::string_view text)
{
    return 0;
}

int Font<Console::CAFE>::GetWidth(uint32_t glyph)
{
    return 0;
}

bool Font<Console::CAFE>::HasGlyphs(std::string_view text) const
{
    return false;
}

bool Font<Console::CAFE>::HasGlyph(uint32_t glyph) const
{
    return false;
}

void Font<Console::CAFE>::GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                                  std::vector<std::string>& lines, std::vector<int>* line_widths)
{}

void Font<Console::CAFE>::GetWrap(const ColoredCodepoints& codepoints, float wraplimit,
                                  std::vector<ColoredCodepoints>& lines,
                                  std::vector<int>* linewidths)
{}
