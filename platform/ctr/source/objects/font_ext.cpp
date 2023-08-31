#include <objects/font_ext.hpp>

#include <modules/graphics_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>
#include <utilities/functions.hpp>

#include <algorithm>
#include <numeric>

using namespace love;

Font<Console::CTR>::Font(Rasterizer<Console::CTR>* rasterizer, const SamplerState& state) :
    Font<>::Font()
{
    this->rasterizers = std::vector<StrongRasterizer>({ rasterizer });
    this->height      = this->rasterizers[0]->GetHeight();

    const auto minFilter = Renderer<Console::CTR>::filterModes.Find(state.minFilter);
    const auto magFilter = Renderer<Console::CTR>::filterModes.Find(state.magFilter);

    this->LoadVolatile();
}

bool Font<Console::CTR>::LoadVolatile()
{
    this->glyphs.clear();
    this->textures.clear();
    this->CreateTexture();

    return true;
}

Font<Console::CTR>::~Font()
{}

void Font<Console::CTR>::SetFallbacks(const std::vector<Font<Console::CTR>*>& fallbacks)
{}

void Font<Console::CTR>::CreateTexture()
{
    auto* font            = this->rasterizers[0]->GetFont();
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

Rasterizer<Console::CTR>::GlyphSheetInfo& Font<Console::CTR>::GetGlyphSheetInfo(uint32_t glyph)
{
    if (glyph == Font::TAB_GLYPH && this->useSpacesAsTab)
        return this->rasterizers[0]->GetSheetInfo(SPACE_GLYPH);

    for (const auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(glyph))
            return rasterizer->GetSheetInfo(glyph);
    }

    return this->rasterizers[0]->GetSheetInfo(glyph);
}

const Font<Console::CTR>::Glyph<C3D_Tex, 0x06>& Font<Console::CTR>::AddGlyph(uint32_t glyph)
{
    StrongReference<GlyphData> data(this->GetRasterizerGlyphData(glyph));

    auto width  = (float)data->GetWidth();
    auto height = (float)data->GetHeight();

    Glyph<C3D_Tex, 0x06> _glyph {};

    _glyph.texture = nullptr;
    _glyph.spacing = std::floor(data->GetAdvance());
    std::fill_n(_glyph.vertices.data(), 6, vertex::Vertex {});

    if (width > 0 && height > 0)
    {
        const auto& sheetInfo = this->GetGlyphSheetInfo(glyph);

        _glyph.sheet   = sheetInfo.index;
        _glyph.texture = &this->textures[_glyph.sheet];

        const auto offset = 1.0f;
        const auto color  = Color(Color::WHITE).array();

        const auto left   = sheetInfo.left;
        const auto top    = sheetInfo.top;
        const auto right  = sheetInfo.right;
        const auto bottom = sheetInfo.bottom;

        // clang-format off
        const std::array<Vertex, 0x06> vertices = 
        {
            /*        x        y       z                u      v      */
            Vertex {{ 0,       0,      0.0f }, color, { left,  top       }},
            Vertex {{ 0,       height, 0.0f }, color, { left,  bottom    }},
            Vertex {{ width,   height, 0.0f }, color, { right, bottom    }},
            Vertex {{ width,   height, 0.0f }, color, { right, bottom    }},
            Vertex {{ width,   0,      0.0f }, color, { right, top       }},
            Vertex {{ 0,       0,      0.0f }, color, { left,  top       }}
        };
        // clang-format on

        for (size_t index = 0; index < 0x06; index++)
        {
            _glyph.vertices[index] = vertices[index];

            _glyph.vertices[index].position[0] += data->GetBearingX();
            _glyph.vertices[index].position[1] += data->GetBearingY();
        }
    }

    this->glyphs[glyph] = _glyph;
    return this->glyphs[glyph];
}

int Font<Console::CTR>::GetWidth(const std::string_view& text)
{
    if (text.size() == 0)
        return 0;

    int maxWidth = 0;

    size_t position = 0;
    int start       = 0;

    while ((position = love::get_line(text, start)) != std::string::npos)
    {
        int width             = 0;
        std::string_view line = text.substr(start, (position - start));

        try
        {
            Utf8Iterator begin(line.begin(), line.begin(), line.end());
            Utf8Iterator end(line.end(), line.begin(), line.end());

            while (begin != end)
            {
                uint32_t codepoint = *begin++;

                if (codepoint == '\r')
                    continue;

                width += this->GetWidth(codepoint);
            }

            start += position + 1;
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: %s", e.what());
        }

        maxWidth = std::max(maxWidth, width);
    }

    return maxWidth;
}

int Font<Console::CTR>::GetWidth(uint32_t glyph)
{
    auto found = this->glyphWidths.find(glyph);

    if (found != this->glyphWidths.end())
        return found->second;

    GlyphData* glyphData     = this->rasterizers[0]->GetGlyphData(glyph);
    this->glyphWidths[glyph] = glyphData->GetAdvance();

    return this->glyphWidths[glyph];
}

const Font<Console::CTR>::Glyph<C3D_Tex, 0x06>& Font<Console::CTR>::FindGlyph(uint32_t glyph)
{
    const auto iterator = this->glyphs.find(glyph);

    if (iterator != this->glyphs.end())
        return iterator->second;

    return this->AddGlyph(glyph);
}

bool Font<Console::CTR>::HasGlyph(uint32_t glyph) const
{
    for (auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(glyph))
            return true;
    }

    return false;
}

bool Font<Console::CTR>::HasGlyphs(const std::string_view& text) const
{
    if (text.size() == 0)
        return false;

    try
    {
        Utf8Iterator begin(text.begin(), text.begin(), text.end());
        Utf8Iterator end(text.end(), text.begin(), text.end());

        while (begin != end)
        {
            uint32_t codepoint = *begin++;

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

float Font<Console::CTR>::GetKerning(const std::string&, const std::string&) const
{
    return 0.0f;
}

float Font<Console::CTR>::GetKerning(uint32_t, uint32_t) const
{
    return 0.0f;
}

void Font<Console::CTR>::SetSamplerState(const SamplerState& state)
{
    Font<>::SetSamplerState(state);

    const auto minFilter = Renderer<Console::CTR>::filterModes.Find(state.minFilter);
    const auto magFilter = Renderer<Console::CTR>::filterModes.Find(state.magFilter);
}

std::vector<Font<>::DrawCommand<C3D_Tex>> Font<Console::CTR>::GenerateVertices(
    const ColoredCodepoints& text, const Color& constantColor,
    std::vector<vertex::Vertex>& vertices, float extraSpacing, Vector2 offset, TextInfo* info)
{
    float dx = offset.x;
    float dy = offset.y;

    float heightOffset = -this->GetBaseline();

    int maxWidth = 0;
    std::vector<Font<>::DrawCommand<C3D_Tex>> commands {};

    /* reserve max possible vertex size */
    size_t startSize = vertices.size();
    vertices.reserve(startSize + text.codepoints.size() * 6);

    uint32_t previousGlyph = 0;
    Color currentColor     = constantColor;

    int colorIndex        = 0;
    const auto colorCount = (int)text.colors.size();

    for (int index = 0; index < (int)text.codepoints.size(); index++)
    {
        /* current glyph to work on */
        const auto glyph = text.codepoints[index];

        /* gamma correct the glyph's color */
        if (colorIndex < colorCount && text.colors[colorIndex].index == index)
        {
            auto glyphColor = text.colors[colorIndex].color;

            glyphColor.r = std::min(std::max(glyphColor.r, 0.0f), 1.0f);
            glyphColor.g = std::min(std::max(glyphColor.g, 0.0f), 1.0f);
            glyphColor.b = std::min(std::max(glyphColor.b, 0.0f), 1.0f);
            glyphColor.a = std::min(std::max(glyphColor.a, 0.0f), 1.0f);

            currentColor = glyphColor;
            colorIndex++;
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

        const auto& glyphData = this->FindGlyph(glyph);
        dx += this->GetKerning(previousGlyph, glyph);

        if (glyphData.texture != nullptr)
        {
            for (int j = 0; j < 0x06; j++)
            {
                vertices.push_back(glyphData.vertices[j]);
                vertices.back().position[0] += dx;
                vertices.back().position[1] += dy + heightOffset;
                vertices.back().color = currentColor.array();
            }

            if (commands.empty() || commands.back().sheet != glyphData.sheet)
            {
                DrawCommand<C3D_Tex> command {};
                command.start   = (int)vertices.size() - 6;
                command.count   = 0;
                command.texture = glyphData.texture;
                command.sheet   = glyphData.sheet;

                commands.push_back(command);
            }

            commands.back().count += 6;
        }

        /* advance the x position */
        dx += glyphData.spacing;

        if (glyph == Font::SPACE_GLYPH && extraSpacing != 0.0f)
            dx = std::floor(dx + extraSpacing);

        previousGlyph = glyph;
    }

    // std::sort(commands.begin(), commands.end(), drawSort);

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

void Font<Console::CTR>::GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                                 std::vector<std::string>& lines, std::vector<int>* lineWidths)
{
    ColoredCodepoints codepoints {};
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<ColoredCodepoints> codepointLines {};
    this->GetWrap(codepoints, wraplimit, codepointLines, lineWidths);

    std::string line;

    for (const ColoredCodepoints& codepoints : codepointLines)
    {
        line.clear();
        line.reserve(codepoints.codepoints.size());

        for (uint32_t codepoint : codepoints.codepoints)
        {
            char character[5] = { '\0' };
            char* end         = utf8::unchecked::append(codepoint, character);
            line.append(character, end - character);
        }

        lines.push_back(line);
    }
}

void Font<Console::CTR>::GetWrap(const ColoredCodepoints& codepoints, float wraplimit,
                                 std::vector<ColoredCodepoints>& lines,
                                 std::vector<int>* linewidths)
{
    float width = 0.0f;

    float widthBeforeLastSpace = 0.0f;
    float widthOfTrailingSpace = 0.0f;

    uint32_t previous  = 0;
    int lastSpaceIndex = -1;

    Color currentColor(1.0f, 1.0f, 1.0f, 1.0f);
    bool addCurrentColor  = false;
    int currentColorIndex = -1;
    int endColorIndex     = (int)codepoints.colors.size() - 1;

    ColoredCodepoints wrappedLine {};

    int index = 0;
    while (index < (int)codepoints.codepoints.size())
    {
        uint32_t current = codepoints.codepoints[index];

        /* determine current color */
        if (currentColorIndex < endColorIndex &&
            codepoints.colors[currentColorIndex + 1].index == index)
        {
            currentColor = codepoints.colors[currentColorIndex + 1].color;
            currentColorIndex++;
            addCurrentColor = true;
        }

        /* split at newlines */
        if (current == Font::NEWLINE_GLYPH)
        {
            lines.push_back(wrappedLine);

            /* ignore width of trailing spaces for individual lines */
            if (linewidths)
                linewidths->push_back(width - widthOfTrailingSpace);

            /* keep previously set color */
            addCurrentColor = true;
            width = widthBeforeLastSpace = widthOfTrailingSpace = 0.0f;

            previous       = 0;
            lastSpaceIndex = -1;
            wrappedLine.codepoints.clear();
            wrappedLine.colors.clear();

            index++;

            continue;
        }

        if (current == Font::CARRIAGE_GLYPH)
        {
            index++;
            continue;
        }

        const auto& glyph = this->FindGlyph(current);

        float charWidth = glyph.spacing + this->GetKerning(previous, current);
        float newWidth  = width + charWidth;

        /* wrap once we hit the line limit, except on newlines */
        if (current != Font::SPACE_GLYPH && newWidth > wraplimit)
        {
            /* skip the first character in the line if it exceeds the limit */
            if (wrappedLine.codepoints.empty())
                index++;
            else if (lastSpaceIndex != -1)
            {
                /* 'rewind' to last seen space, if the line contains one */
                while (!wrappedLine.codepoints.empty() &&
                       wrappedLine.codepoints.back() != Font::SPACE_GLYPH)
                {
                    wrappedLine.codepoints.pop_back();
                }

                while (!wrappedLine.colors.empty() &&
                       wrappedLine.colors.back().index >= (int)wrappedLine.codepoints.size())
                {
                    wrappedLine.colors.pop_back();
                }

                /* 'rewind' to the last color */
                for (int colorIndex = currentColorIndex; colorIndex >= 0; colorIndex--)
                {
                    if (codepoints.colors[colorIndex].index <= lastSpaceIndex)
                    {
                        currentColor      = codepoints.colors[colorIndex].color;
                        currentColorIndex = colorIndex;
                        break;
                    }
                }

                width = widthBeforeLastSpace;
                index = lastSpaceIndex;
                index++;
            }

            lines.push_back(wrappedLine);

            if (linewidths)
                linewidths->push_back(width);

            addCurrentColor = true;
            previous        = 0;
            width = widthBeforeLastSpace = widthOfTrailingSpace = 0.0f;

            wrappedLine.codepoints.clear();
            wrappedLine.colors.clear();
            lastSpaceIndex = -1;

            continue;
        }

        if (previous != Font::SPACE_GLYPH && current == Font::SPACE_GLYPH)
            widthBeforeLastSpace = width;

        width    = newWidth;
        previous = current;

        if (addCurrentColor)
        {
            wrappedLine.colors.push_back({ currentColor, (int)wrappedLine.codepoints.size() });
            addCurrentColor = false;
        }

        wrappedLine.codepoints.push_back(current);

        if (current == Font::SPACE_GLYPH)
        {
            lastSpaceIndex       = index;
            widthOfTrailingSpace = charWidth;
        }
        else if (current != Font::NEWLINE_GLYPH)
            widthOfTrailingSpace = 0.0f;

        index++;
    }

    lines.push_back(wrappedLine);

    if (linewidths)
        linewidths->push_back(width - widthOfTrailingSpace);
}

std::vector<Font<>::DrawCommand<C3D_Tex>> Font<Console::CTR>::GenerateVerticesFormatted(
    const ColoredCodepoints& text, const Color& constantColor, float wrap, AlignMode align,
    std::vector<vertex::Vertex>& vertices, TextInfo* info)
{
    wrap = std::max(wrap, 0.0f);

    std::vector<DrawCommand<C3D_Tex>> commands;
    vertices.reserve(text.codepoints.size() * 4);

    std::vector<int> widths;
    std::vector<ColoredCodepoints> lines;

    this->GetWrap(text, wrap, lines, &widths);

    float y        = 0.0f;
    float maxWidth = 0.0f;

    for (int index = 0; index < (int)lines.size(); index++)
    {
        const auto& line = lines[index];

        float width = (float)widths[index];

        Vector2 offset(0.0f, floorf(y));
        float extraSpacing = 0.0f;

        maxWidth = std::max(width, maxWidth);

        switch (align)
        {
            case ALIGN_RIGHT:
            {
                offset.x = floorf(wrap - width);
                break;
            }
            case ALIGN_CENTER:
            {
                offset.x = floorf((wrap - width) / 2.0f);
                break;
            }
            case ALIGN_JUSTIFY:
            {
                float spaces = std::count(line.codepoints.begin(), line.codepoints.end(), ' ');

                if (width < wrap && spaces >= 1)
                    extraSpacing = (wrap - width) / spaces;
                else
                    extraSpacing = 0.0f;

                break;
            }
            case ALIGN_LEFT:
            default:
                break;
        }

        std::vector<DrawCommand<C3D_Tex>> newCommands =
            this->GenerateVertices(line, constantColor, vertices, extraSpacing, offset);

        if (!newCommands.empty())
        {
            auto first = newCommands.begin();

            /*
            ** if the first draw command has the same texture
            ** as the last in the existing list and vertices
            ** are in-order, we can combine them
            */
            if (!commands.empty())
            {
                auto previous = commands.back();

                size_t total = (previous.start + previous.count);
                if (previous.texture == first->texture && (int)total == first->start)
                {
                    commands.back().count += first->count;
                    ++first;
                }
            }

            commands.insert(commands.end(), first, newCommands.end());
        }
        y += this->GetHeight() * this->GetLineHeight();
    }

    if (info != nullptr)
    {
        info->width  = (int)maxWidth;
        info->height = (int)y;
    }

    return commands;
}

void Font<Console::CTR>::Print(Graphics<Console::CTR>& graphics, const ColoredStrings& text,
                               const Matrix4<Console::CTR>& transform, const Color& color)
{
    ColoredCodepoints codepoints {};
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<vertex::Vertex> vertices {};
    auto commands = this->GenerateVertices(codepoints, color, vertices);

    this->Render(graphics, transform, commands, vertices);
}

void Font<Console::CTR>::Printf(Graphics<Console::CTR>& graphics, const ColoredStrings& text,
                                float wrap, AlignMode alignment,
                                const Matrix4<Console::CTR>& matrix, const Color& color)
{
    ColoredCodepoints codepoints {};
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<vertex::Vertex> vertices {};
    std::vector<DrawCommand<C3D_Tex>> commands =
        this->GenerateVerticesFormatted(codepoints, color, wrap, alignment, vertices);

    this->Render(graphics, matrix, commands, vertices);
}

void Font<Console::CTR>::Render(Graphics<Console::CTR>& graphics,
                                const Matrix4<Console::CTR>& matrix,
                                const std::vector<DrawCommand<C3D_Tex>>& commands,
                                const std::vector<vertex::Vertex>& vertices)
{
    if (vertices.empty() || commands.empty())
        return;

    const auto& transform = graphics.GetTransform();
    bool is2D             = transform.IsAffine2DTransform();

    Matrix4<Console::CTR> translated(transform, matrix);

    for (const auto& command : commands)
    {
        love::DrawCommand<Console::CTR> drawCommand(command.count, vertex::PRIMITIVE_TRIANGLES);
        drawCommand.handles = { command.texture };
        drawCommand.format  = CommonFormat::FONT;

        translated.TransformXY(drawCommand.Positions().get(), &vertices[command.start],
                               command.count);

        drawCommand.FillVertices(&vertices[command.start]);
        Renderer<Console::CTR>::Instance().Render(drawCommand);
    }
}