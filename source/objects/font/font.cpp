#include <modules/graphics_ext.hpp>

#include <objects/rasterizer/rasterizer.hpp>
#include <objects/texture_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>
#include <utilities/functions.hpp>

#include <utf8.h>

#include <algorithm>
#include <limits>

using namespace love;
using namespace love::vertex;

Font::Font(Rasterizer* rasterizer, const SamplerState& state) :
    textureWidth(128),
    textureHeight(128),
    textureCacheID(0),
    shaper(rasterizer->NewTextShaper(), Acquire::NORETAIN),
    samplerState {},
    dpiScale(rasterizer->GetDPIScale())
{
    this->samplerState.minFilter     = state.minFilter;
    this->samplerState.magFilter     = state.magFilter;
    this->samplerState.maxAnisotropy = state.maxAnisotropy;

    /* find the best texture size match for the font size */
    while (true)
    {
        const auto height = this->shaper->GetHeight();
        if ((height * 0.8) * height * 30 <= this->textureWidth * this->textureHeight)
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

    auto* graphics = Module::GetInstance<Graphics<>>(Module::M_GRAPHICS);

    auto usage = PIXELFORMAT_USAGE_FLAGS_SAMPLE;
    if (format == PIXELFORMAT_LA8_UNORM && !graphics->IsPixelFormatSupported(format, usage))
        format = PIXELFORMAT_RGBA8_UNORM;

    this->LoadVolatile();
    Font::fontCount++;
}

int Font::GetAscent() const
{
    return this->shaper->GetAscent();
}

int Font::GetDescent() const
{
    return this->shaper->GetDescent();
}

float Font::GetHeight() const
{
    return this->shaper->GetHeight();
}

float Font::GetLineHeight() const
{
    return this->shaper->GetLineHeight();
}

void Font::SetLineHeight(float height)
{
    this->shaper->SetLineHeight(height);
}

float Font::GetBaseline() const
{
    return this->shaper->GetBaseline();
}

Font::TextureSize Font::GetNextTextureSize() const
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

bool Font::LoadVolatile()
{
    this->textureCacheID++;
    this->glyphs.clear();
    this->textures.clear();
    this->CreateTexture();

    return true;
}

void Font::UnloadVolatile()
{
    this->glyphs.clear();
    this->textures.clear();
}

#if !defined(__3DS__)
void Font::CreateTexture()
{
    auto* graphics = Module::GetInstance<Graphics<Console::Which>>(Module::M_GRAPHICS);

    Texture<Console::Which>* texture = nullptr;

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

        if (this->shaper->GetRasterizers()[0]->GetDataType() == Rasterizer::DATA_TRUETYPE)
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
        std::vector<TextShaper::GlyphIndex> glyphsToAdd;

        for (const auto& pair : this->glyphs)
            glyphsToAdd.push_back(TextShaper::UnpackGlyphIndex(pair.first));

        this->glyphs.clear();

        for (auto glyph : glyphsToAdd)
            this->AddGlyph(glyph);
    }
}
#else
void Font::CreateTexture()
{
    CFNT_s* font          = (CFNT_s*)this->shaper->GetRasterizers()[0]->GetHandle();
    const auto* glyphInfo = fontGetGlyphInfo(font);

    this->textures.reserve(glyphInfo->nSheets);

    const auto MIN_MAG = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR);

    const auto WRAP =
        GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_BORDER) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_BORDER);

    for (size_t index = 0; index < glyphInfo->nSheets; index++)
    {
        this->textures.push_back(C3D_Tex {});

        C3D_Tex* texture  = &this->textures[index];
        texture->data     = fontGetGlyphSheetTex(font, index);
        texture->fmt      = (GPU_TEXCOLOR)glyphInfo->sheetFmt;
        texture->size     = glyphInfo->sheetSize;
        texture->width    = glyphInfo->sheetWidth;
        texture->height   = glyphInfo->sheetHeight;
        texture->param    = MIN_MAG | WRAP;
        texture->border   = 0;
        texture->lodParam = 0;
    }
}
#endif

GlyphData* Font::GetRasterizerGlyphData(TextShaper::GlyphIndex glyphIndex, float& dpiScale)
{
    const auto& rasterizer = this->shaper->GetRasterizers()[glyphIndex.rasterizerIndex];
    dpiScale               = rasterizer->GetDPIScale();

    return rasterizer->GetGlyphDataForIndex(glyphIndex.index);
}

const Font::Glyph& Font::AddGlyph(TextShaper::GlyphIndex glyphIndex)
{
    float dpiScale = this->GetDPIScale();
    StrongReference<GlyphData> data(this->GetRasterizerGlyphData(glyphIndex, dpiScale),
                                    Acquire::NORETAIN);

    auto width  = data->GetWidth();
    auto height = data->GetHeight();

    float offset = 0.0f;

    float left   = 0.0f;
    float right  = 0.0f;
    float bottom = 0.0f;
    float top    = 0.0f;

    Glyph _glyph {};
    _glyph.texture = nullptr;

    std::fill_n(_glyph.vertices.data(), 6, Vertex {});

#if !defined(__3DS__)
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
            return this->AddGlyph(glyphIndex);
        }
    }

    /* don't waste space on empty glyphs */
    if (width > 0 && height > 0)
    {
        Texture<Console::Which>* texture = this->textures.back();
        _glyph.texture                   = texture;

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

        offset = 1.0f;

        /* texture coordinates */
        left   = (float)((x - offset) / _width);
        top    = (float)((y - offset) / _height);
        right  = (float)((x + width + offset) / _width);
        bottom = (float)((y + height + offset) / _height);
    }
#else
    if (width > 0 && height > 0)
    {
        const auto& sheetInfo = data->GetGlyphSheetInfo();

        _glyph.sheet   = sheetInfo.index;
        _glyph.texture = &this->textures[_glyph.sheet];

        left   = sheetInfo.left;
        top    = sheetInfo.top;
        right  = sheetInfo.right;
        bottom = sheetInfo.bottom;
    }
#endif

    if (width > 0 && height > 0)
    {
        const auto color = Color(Color::WHITE).array();

        // clang-format off
        const std::array<Vertex, 0x06> vertices =
        {
            /*        x                            y                             z                u      v      */
            Vertex {{ -offset,                     -offset,                      0.0f }, color, { left,  top    }},
            Vertex {{ -offset,                     (height + offset) / dpiScale, 0.0f }, color, { left,  bottom }},
            Vertex {{ (width + offset) / dpiScale, (height + offset) / dpiScale, 0.0f }, color, { right, bottom }},
            Vertex {{ (width + offset) / dpiScale, (height + offset) / dpiScale, 0.0f }, color, { right, bottom }},
            Vertex {{ (width + offset) / dpiScale, -offset,                      0.0f }, color, { right, top    }},
            Vertex {{ -offset,                     -offset,                      0.0f }, color, { left,  top    }}
        };
        // clang-format on

        /* copy the vertex data into the Glyph and set proper bearing */
        _glyph.vertices = vertices;

        for (size_t index = 0; index < vertices.size(); index++)
        {
            _glyph.vertices[index].position[0] += data->GetBearingX() / dpiScale;
            auto bearingY = data->GetBearingY() / dpiScale;

            if (!Console::Is(Console::CTR))
                bearingY = -bearingY;

            _glyph.vertices[index].position[1] += bearingY;
        }

        this->textureX += width + Font::TEXTURE_PADDING;
        this->rowHeight = std::max(this->rowHeight, height + Font::TEXTURE_PADDING);
    }

    const auto packedIndex    = TextShaper::PackGlyphIndex(glyphIndex);
    this->glyphs[packedIndex] = _glyph;
    return this->glyphs[packedIndex];
}

const Font::Glyph& Font::FindGlyph(TextShaper::GlyphIndex glyphIndex)
{
    const auto packedIndex = TextShaper::PackGlyphIndex(glyphIndex);
    const auto iterator    = this->glyphs.find(packedIndex);

    if (iterator != this->glyphs.end())
        return iterator->second;

    return this->AddGlyph(glyphIndex);
}

float Font::GetKerning(uint32_t left, uint32_t right)
{
    return this->shaper->GetKerning(left, right);
}

float Font::GetKerning(const std::string& left, const std::string& right)
{
    return this->shaper->GetKerning(left, right);
}

struct PositionedGlyph
{
    const TextShaper::GlyphPosition* position;
    const Font::Glyph* glyph;
    Color color;
};

static bool sortGlyphs(const PositionedGlyph& left, const PositionedGlyph& right)
{
    if (Console::Is(Console::CTR))
    {
        const auto result = left.glyph->texture - right.glyph->texture;
        return result < 0;
    }

    return left.glyph->texture < right.glyph->texture;
}

static inline bool isSimilarCommand(const Font::DrawCommand& previous,
                                    std::vector<love::Font::DrawCommand>::iterator current)
{
    if (Console::Is(Console::CTR))
        return previous.sheet == current->sheet;

    return previous.texture == current->texture;
}

std::vector<Font::DrawCommand> Font::GenerateVertices(const ColoredCodepoints& codepoints,
                                                      Range range, const Color& color,
                                                      std::vector<Vertex>& vertices,
                                                      float extraSpacing, Vector2 offset,
                                                      TextShaper::TextInfo* info)
{
    std::vector<TextShaper::GlyphPosition> positions {};
    std::vector<IndexedColor> colors {};

    this->shaper->ComputeGlyphPositions(codepoints, range, offset, extraSpacing, &positions,
                                        &colors, info);

    size_t vertexStartSize = vertices.size();

    const auto linearColor = Graphics<>::GammaCorrectColor(color);
    auto currentColor      = linearColor;

    int currentColorIndex = 0;
    int colorCount        = colors.size();

    std::vector<PositionedGlyph> glyphs {};
    glyphs.reserve(positions.size());

    for (int index = 0; index < (int)positions.size(); index++)
    {
        const auto& info = positions[index];
        uint32_t cacheId = this->textureCacheID;

        const auto& glyph = this->FindGlyph(info.glyphIndex);

        if (cacheId != this->textureCacheID)
        {
            index = -1;
            glyphs.clear();
            vertices.resize(vertexStartSize);
            currentColorIndex = 0;
            currentColor      = color;
            continue;
        }

        if (currentColorIndex < colorCount && colors[currentColorIndex].index == index)
        {
            auto newColor = colors[currentColorIndex].color;

            newColor.r = std::clamp(newColor.r, 0.0f, 1.0f);
            newColor.g = std::clamp(newColor.g, 0.0f, 1.0f);
            newColor.b = std::clamp(newColor.b, 0.0f, 1.0f);
            newColor.a = std::clamp(newColor.a, 0.0f, 1.0f);

            Graphics<>::GammaCorrectColor(newColor);
            newColor *= linearColor;
            Graphics<>::UnGammaCorrectColor(newColor);

            currentColor = newColor;
            currentColorIndex++;
        }

        glyphs.emplace_back(&info, &glyph, currentColor);
    }

    std::sort(glyphs.begin(), glyphs.end(), sortGlyphs);

    std::vector<DrawCommand> commands {};

    for (int index = 0; index < (int)glyphs.size(); index++)
    {
        const auto& info = *glyphs[index].position;

        const auto& glyph = *glyphs[index].glyph;
        const auto& color = glyphs[index].color;

        if (glyph.texture != nullptr)
        {
            for (int j = 0; j < (int)glyph.vertices.size(); j++)
            {
                vertices.push_back(glyph.vertices[j]);
                vertices.back().position[0] += info.position.x;
                vertices.back().position[1] += info.position.y;
                vertices.back().color = color.array();
            }

            // check if a glyph texture or sheet has changed since last iteration
            if (commands.empty() || commands.back().texture != glyph.texture)
            {
                DrawCommand& command = commands.emplace_back();
                command.start        = (int)vertices.size() - 0x06;
                command.count        = 0x0;
                command.texture      = glyph.texture;
            }

            commands.back().count += 0x06;
        }
    }

    return commands;
}

std::vector<Font::DrawCommand> Font::GenerateVerticesFormatted(
    const ColoredCodepoints& codepoints, const Color& color, float wrap, AlignMode align,
    std::vector<vertex::Vertex>& vertices, TextShaper::TextInfo* info)
{
    wrap = std::max(wrap, 0.0f);

    uint32_t cacheId = this->textureCacheID;

    std::vector<DrawCommand> drawCommands {};
    vertices.reserve(codepoints.cps.size() * 6);

    std::vector<Range> ranges {};
    std::vector<int> widths {};

    this->shaper->GetWrap(codepoints, wrap, ranges, &widths);

    float y        = 0.0f;
    float maxWidth = 0.0f;

    for (int index = 0; index < (int)ranges.size(); index++)
    {
        const auto& range = ranges[index];

        if (!range.isValid())
        {
            y += this->GetHeight() * this->GetLineHeight();
            continue;
        }

        float width = widths[index];
        Vector2 offset(0.0f, std::floor(y));
        float extraSpacing = 0.0f;

        maxWidth = std::max(width, maxWidth);

        switch (align)
        {
            case ALIGN_RIGHT:
            {
                offset.x = std::floor(wrap - width);
                break;
            }
            case ALIGN_CENTER:
            {
                offset.x = std::floor((wrap - width) / 2.0f);
                break;
            }
            case ALIGN_JUSTIFY:
            {
                auto start = codepoints.cps.begin() + range.getOffset();
                auto end   = start + range.getSize();

                float numSpaces = std::count(start, end, ' ');

                if (width < wrap && numSpaces >= 1)
                    extraSpacing = (wrap - width) / numSpaces;
                else
                    extraSpacing = 0.0f;

                break;
            }
            case ALIGN_LEFT:
            default:
                break;
        }

        auto newCommands =
            this->GenerateVertices(codepoints, range, color, vertices, extraSpacing, offset);

        if (!newCommands.empty())
        {
            auto firstCommand = newCommands.begin();

            if (!drawCommands.empty())
            {
                auto prevCommand = drawCommands.back();
                bool isSimilar   = isSimilarCommand(prevCommand, firstCommand);

                if (isSimilar && (prevCommand.start + prevCommand.count) == firstCommand->start)
                {
                    drawCommands.back().count += firstCommand->count;
                    ++firstCommand;
                }
            }

            /* append new commands to the built list */
            drawCommands.insert(drawCommands.end(), firstCommand, newCommands.end());
        }

        y += this->GetHeight() * this->GetLineHeight();
    }

    if (info != nullptr)
    {
        info->width  = maxWidth;
        info->height = y;
    }

    if (cacheId != this->textureCacheID)
    {
        vertices.clear();
        drawCommands = this->GenerateVerticesFormatted(codepoints, color, wrap, align, vertices);
    }

    return drawCommands;
}

void Font::Render(Graphics<>& graphics, const Matrix4& transform,
                  const std::vector<DrawCommand>& drawCommands, const std::vector<Vertex>& vertices)
{
    if (vertices.empty() || drawCommands.empty())
        return;

    Matrix4 matrix(graphics.GetTransform(), transform);

    for (const auto& command : drawCommands)
    {
        /* total vertices for the quads */
        CommonFormat format = CommonFormat::TEXTURE;
        if (Console::Is(Console::CTR))
            format = CommonFormat::FONT;

        auto shader = Shader<>::STANDARD_TEXTURE;
        if (Console::Is(Console::CTR))
            shader = Shader<>::STANDARD_DEFAULT;

        love::DrawCommand drawCommand(command.count, shader, format);
        drawCommand.type = PrimitiveType::PRIMITIVE_TRIANGLES;

        /* texture to use - single texture */
        drawCommand.handles = { command.texture };

        const auto start = command.start;
        const auto count = command.count;

        std::memcpy(drawCommand.Vertices().get(), &vertices[start], drawCommand.size);
        matrix.TransformXY(drawCommand.GetVertices(), std::span(&vertices[start], count));

        Renderer<Console::Which>::Instance().Render(drawCommand);
    }
}

void Font::Print(Graphics<>& graphics, const ColoredStrings& text, const Matrix4& matrix,
                 const Color& color)
{
    ColoredCodepoints codepoints {};
    love::GetCodepointsFromString(text, codepoints);

    std::vector<Vertex> vertices {};
    auto commands = this->GenerateVertices(codepoints, Range(), color, vertices);

    this->Render(graphics, matrix, commands, vertices);
}

void Font::Printf(Graphics<>& graphics, const ColoredStrings& text, float wrap, AlignMode alignment,
                  const Matrix4& matrix, const Color& color)
{
    ColoredCodepoints codepoints {};
    love::GetCodepointsFromString(text, codepoints);

    std::vector<Vertex> vertices {};
    auto commands = this->GenerateVerticesFormatted(codepoints, color, wrap, alignment, vertices);

    this->Render(graphics, matrix, commands, vertices);
}

#if !defined(__3DS__)
void Font::SetSamplerState(const SamplerState& state)
{
    for (const auto& texture : this->textures)
        texture->SetSamplerState(state);
}
#else
void Font::SetSamplerState(const SamplerState& state)
{
    // this->samplerState = state;
    // for (auto& texture : this->textures)
}
#endif

int Font::GetWidth(const std::string& text)
{
    return this->shaper->GetWidth(text);
}

int Font::GetWidth(uint32_t glyph)
{
    return this->shaper->GetGlyphAdvance(glyph);
}

bool Font::HasGlyphs(const std::string& text) const
{
    return this->shaper->HasGlyphs(text);
}

bool Font::HasGlyph(uint32_t glyph) const
{
    return this->shaper->HasGlyph(glyph);
}

void Font::GetWrap(const ColoredStrings& codepoints, float wraplimit,
                   std::vector<std::string>& lines, std::vector<int>* lineWidths)
{
    this->shaper->GetWrap(codepoints, wraplimit, lines, lineWidths);
}

void Font::GetWrap(const ColoredCodepoints& codepoints, float wraplimit, std::vector<Range>& ranges,
                   std::vector<int>* linewidths)
{
    this->shaper->GetWrap(codepoints, wraplimit, ranges, linewidths);
}
