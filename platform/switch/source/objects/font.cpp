#include "objects/font/font.h"

#include "modules/font/fontmodule.h"
#include "modules/graphics/graphics.h"

#include "common/matrix.h"
#include "deko3d/graphics.h"

#include "deko3d/deko.h"
#include "utf8/utf8.h"

using namespace vertex;

using namespace love;

#define FONT_MODULE() (Module::GetInstance<FontModule>(Module::M_FONT))

Font::Font(Rasterizer* r, const Texture::Filter& filter) :
    common::Font(filter),
    rasterizers({ r }),
    textureWidth(128),
    textureHeight(128),
    useSpacesAsTab(false),
    textureCacheID(0)
{
    this->dpiScale = rasterizers[0]->GetDPIScale();
    this->height   = rasterizers[0]->GetHeight();

    this->lineHeight = 1.0f;

    this->filter        = filter;
    this->filter.mipmap = Texture::FILTER_NONE;

    while (true)
    {
        if ((this->height * 0.8) * this->height * 30 <= this->textureWidth * this->textureHeight)
            break;

        TextureSize nextsize = this->GetNextTextureSize();

        if (nextsize.width <= textureWidth && nextsize.height <= textureHeight)
            break;

        this->textureWidth  = nextsize.width;
        this->textureHeight = nextsize.height;
    }

    /* No tab character in the Rasterizer. */
    if (!r->HasGlyph(9))
        this->useSpacesAsTab = true;

    this->textureCacheID++;

    this->glyphs.clear();
    this->images.clear();

    this->CreateTexture();
}

Font::~Font()
{
    this->glyphs.clear();
    this->images.clear();
}

void Font::SetFilter(const Texture::Filter& filter)
{
    for (const auto& image : this->images)
        image->SetFilter(filter);

    this->filter = filter;
}

Font::TextureSize Font::GetNextTextureSize() const
{
    TextureSize size = { this->textureWidth, this->textureHeight };

    int maxSize = 2048;

    int maxWidth  = std::min(8192, maxSize);
    int maxHeight = std::min(4096, maxSize);

    if (size.width * 2 <= maxWidth || size.height * 2 <= maxHeight)
    {
        // {128, 128} -> {256, 128} -> {256, 256} -> {512, 256} -> etc.
        if (size.width == size.height)
            size.width *= 2;
        else
            size.height *= 2;
    }

    return size;
}

void Font::CreateTexture()
{
    auto gfx = Module::GetInstance<deko3d::Graphics>(Module::M_GRAPHICS);

    TextureSize size     = { this->textureWidth, this->textureHeight };
    TextureSize nextSize = this->GetNextTextureSize();

    love::Image* texture = nullptr;
    bool recreatetexture = false;

    /*
    ** If we have an existing texture already, we'll try replacing it with a
    ** larger-sized one rather than creating a second one. Having a single
    ** texture reduces texture switches and draw calls when rendering.
    */
    if ((nextSize.width > size.width || nextSize.height > size.height) && !this->images.empty())
    {
        recreatetexture = true;
        size            = nextSize;
        images.pop_back();
    }

    texture =
        gfx->NewImage(love::Texture::TEXTURE_2D, PIXELFORMAT_RGBA8, size.width, size.height, 1);
    texture->SetFilter(this->filter);

    {
        size_t bytesPerPixel = love::GetPixelFormatSize(PIXELFORMAT_RGBA8);
        size_t pixelCount    = size.width * size.height;

        std::vector<uint8_t> empty(pixelCount * bytesPerPixel, 0);
        Rect rect = { 0, 0, size.width, size.height };

        texture->ReplacePixels(empty.data(), empty.size(), rect);
    }

    this->images.emplace_back(texture, Acquire::NORETAIN);

    this->textureWidth  = size.width;
    this->textureHeight = size.height;

    this->rowHeight = this->textureX = this->textureY = this->TEXTURE_PADDING;

    if (recreatetexture)
    {
        this->textureCacheID++;

        std::vector<uint32_t> glyphsToAdd;

        for (const auto& glyphPair : this->glyphs)
            glyphsToAdd.push_back(glyphPair.first);

        this->glyphs.clear();

        for (uint32_t glyph : glyphsToAdd)
            this->AddGlyph(glyph);
    }
}

uint32_t Font::GetTextureCacheID()
{
    return this->textureCacheID;
}

love::GlyphData* Font::GetRasterizerGlyphData(uint32_t glyph)
{
    /* Use spaces for the tab 'glyph' */
    if (glyph == 9 && useSpacesAsTab)
    {
        love::GlyphData* spacegd = this->rasterizers[0]->GetGlyphData(32);

        love::GlyphData::GlyphMetrics gm = {};

        gm.advance  = spacegd->GetAdvance() * SPACES_PER_TAB;
        gm.bearingX = spacegd->GetBearingX();
        gm.bearingY = spacegd->GetBearingY();

        spacegd->Release();

        return new love::GlyphData(glyph, gm);
    }

    for (const auto& r : rasterizers)
    {
        if (r->HasGlyph(glyph))
            return r->GetGlyphData(glyph);
    }

    return rasterizers[0]->GetGlyphData(glyph);
}

float Font::GetDPIScale() const
{
    return this->dpiScale;
}

const Font::Glyph& Font::FindGlyph(uint32_t glyph)
{
    const auto it = this->glyphs.find(glyph);

    if (it != this->glyphs.end())
        return it->second;

    return this->AddGlyph(glyph);
}

bool Font::HasGlyph(uint32_t glyph) const
{
    for (const StrongReference<Rasterizer>& r : rasterizers)
    {
        if (r->HasGlyph(glyph))
            return true;
    }

    return false;
}

static inline uint16_t norm16(double n)
{
    return uint16_t(n * 0xFFFF);
}

const Font::Glyph& Font::AddGlyph(uint32_t glyph)
{
    love::StrongReference<love::GlyphData> gd(this->GetRasterizerGlyphData(glyph),
                                              Acquire::NORETAIN);

    int width  = gd->GetWidth();
    int height = gd->GetHeight();

    if (width + this->TEXTURE_PADDING * 2 < this->textureWidth &&
        height + this->TEXTURE_PADDING * 2 < this->textureHeight)
    {
        if (this->textureX + width + this->TEXTURE_PADDING > this->textureWidth)
        {
            // Out of space - new row!
            this->textureX = this->TEXTURE_PADDING;
            this->textureY += this->rowHeight;
            this->rowHeight = this->TEXTURE_PADDING;
        }

        if (this->textureY + height + this->TEXTURE_PADDING > this->textureHeight)
        {
            /* Totally out of space - new texture! */
            this->CreateTexture();

            /* Ensure last glyph is re-added */
            return this->AddGlyph(glyph);
        }
    }

    Glyph g;

    g.texture = 0;
    g.spacing = floorf(gd->GetAdvance() / this->dpiScale + 0.5f);

    std::fill_n(g.vertices, 4, GlyphVertex {});

    /* Don't waste space on empty glyphs */
    if (width > 0 && height > 0)
    {
        Image* image = images.back();
        g.texture    = image;

        Rect rect = { this->textureX, this->textureY, gd->GetWidth(), gd->GetHeight() };

        image->ReplacePixels(gd->GetData(), gd->GetSize(), rect);

        double tX = (double)this->textureX, tY = (double)this->textureY;
        double tWidth = (double)this->textureWidth, tHeight = (double)this->textureHeight;

        Colorf c(1.0f, 1.0f, 1.0f, 1.0f);

        // Extrude the quad borders by 1 pixel. We have an extra pixel of
        // transparent padding in the texture atlas, so the quad extrusion will
        // add some antialiasing at the edges of the quad.
        int o = 1;

        // 0---3
        // |   |
        // 1---2
        const GlyphVertex verts[4] = {
            { float(-o), float(-o), norm16((tX - o) / tWidth), norm16((tY - o) / tHeight), c },
            { float(-o), (height + o) / this->dpiScale, norm16((tX - o) / tWidth),
              norm16((tY + height + o) / tHeight), c },
            { (width + o) / this->dpiScale, (height + o) / this->dpiScale,
              norm16((tX + width + o) / tWidth), norm16((tY + height + o) / tHeight), c },
            { (width + o) / this->dpiScale, float(-o), norm16((tX + width + o) / tWidth),
              norm16((tY - o) / tHeight), c },
        };

        // Copy vertex data to the glyph
        // and set proper bearing.
        for (int i = 0; i < 4; i++)
        {
            g.vertices[i] = verts[i];

            g.vertices[i].x += gd->GetBearingX() / this->dpiScale;
            g.vertices[i].y -= gd->GetBearingY() / this->dpiScale;
        }

        this->textureX += width + this->TEXTURE_PADDING;
        this->rowHeight = std::max(this->rowHeight, height + this->TEXTURE_PADDING);
    }

    this->glyphs[glyph] = g;
    return this->glyphs[glyph];
}

void Font::GetCodepointsFromString(const std::string& text, Codepoints& codepoints)
{
    codepoints.reserve(text.size());

    try
    {
        utf8::iterator<std::string::const_iterator> i(text.begin(), text.begin(), text.end());
        utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

        while (i != end)
        {
            uint32_t g = *i++;
            codepoints.push_back(g);
        }
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }
}

void Font::GetCodepointsFromString(const std::vector<ColoredString>& strings,
                                   ColoredCodepoints& codepoints)
{
    if (strings.empty())
        return;

    codepoints.codes.reserve(strings[0].string.size());

    for (const ColoredString& coloredString : strings)
    {
        if (coloredString.string.size() == 0)
            continue;

        IndexedColor iColor = { coloredString.color, (int)codepoints.codes.size() };
        codepoints.colors.push_back(iColor);

        Font::GetCodepointsFromString(coloredString.string, codepoints.codes);
    }

    if (codepoints.colors.size() == 1)
    {
        IndexedColor iColor = codepoints.colors[0];

        if (iColor.index == 0 && iColor.color == Colorf(1.0f, 1.0f, 1.0f, 1.0f))
            codepoints.colors.pop_back();
    }
}

std::vector<Font::DrawCommand> Font::GenerateVertices(const ColoredCodepoints& codepoints,
                                                      const Colorf& constantColor,
                                                      std::vector<GlyphVertex>& glyphVertices,
                                                      float extra_spacing, Vector2 offset,
                                                      TextInfo* info)
{
    /*
    ** Spacing counter and
    ** newline handling.
    */
    float dx = offset.x;
    float dy = offset.y;

    float heightoffset = 0.0f;

    if (this->rasterizers[0]->GetDataType() == Rasterizer::DATA_TRUETYPE)
        heightoffset = this->GetBaseline();

    int maxwidth = 0;
    std::vector<Font::DrawCommand> commands;

    // Pre-allocate space for the maximum possible number of vertices.
    size_t vertstartsize = glyphVertices.size();
    glyphVertices.reserve(vertstartsize + codepoints.codes.size() * 4);

    uint32_t prevGlyph = 0;

    Colorf linearConstant = Graphics::GammaCorrectColor(constantColor);
    Colorf currentColor   = constantColor;

    int currentColorIndex = -1;
    int numColors         = (int)codepoints.colors.size();

    for (int i = 0; i < (int)codepoints.codes.size(); i++)
    {
        uint32_t glyphChar = codepoints.codes[i];

        if (currentColorIndex + 1 < numColors &&
            codepoints.colors[currentColorIndex + 1].index == i)
        {
            Colorf c = codepoints.colors[++currentColorIndex].color;

            c.r = std::min(std::max(c.r, 0.0f), 1.0f);
            c.g = std::min(std::max(c.g, 0.0f), 1.0f);
            c.b = std::min(std::max(c.b, 0.0f), 1.0f);
            c.a = std::min(std::max(c.a, 0.0f), 1.0f);

            Graphics::GammaCorrectColor(c);
            c *= linearConstant;
            Graphics::UnGammaCorrectColor(c);

            currentColor = c;
        }

        if (glyphChar == '\n')
        {
            if (dx > maxwidth)
                maxwidth = (int)dx;

            // Wrap newline, but do not print it.
            dy += floorf(this->GetHeight() * this->GetLineHeight() + 0.5f);
            dx = offset.x;

            prevGlyph = 0;

            continue;
        }

        /* Ignore carriage returns */
        if (glyphChar == '\r')
            continue;

        uint32_t cacheID = this->textureCacheID;

        const Glyph& glyph = this->FindGlyph(glyphChar);

        /*
        ** If findGlyph invalidates the texture cache
        ** restart the loop.
        */
        if (cacheID != this->textureCacheID)
        {
            i        = -1; // The next iteration will increment this to 0.
            maxwidth = 0;

            dx = offset.x;
            dy = offset.y;

            commands.clear();

            glyphVertices.resize(vertstartsize);
            prevGlyph = 0;

            currentColorIndex = -1;
            currentColor      = constantColor;

            continue;
        }

        /* Add kerning to the current horizontal offset. */
        dx += this->GetKerning(prevGlyph, glyphChar);

        if (glyph.texture != nullptr)
        {
            /*
            ** Copy the vertices and set their colors
            ** and relative positions.
            */
            for (int j = 0; j < 4; j++)
            {
                glyphVertices.push_back(glyph.vertices[j]);
                glyphVertices.back().x += dx;
                glyphVertices.back().y += dy + heightoffset;

                glyphVertices.back().color = currentColor;
            }

            /* Check if glyph texture has changed since the last iteration. */
            if (commands.empty() || commands.back().texture != glyph.texture)
            {
                /* Add a new draw command if the texture has changed. */
                DrawCommand cmd;
                cmd.startVertex = (int)glyphVertices.size() - 4;
                cmd.vertexCount = 0;
                cmd.texture     = glyph.texture;

                commands.push_back(cmd);
            }

            commands.back().vertexCount += 4;
        }

        dx += glyph.spacing;

        // Account for extra spacing given to space characters.
        if (glyphChar == ' ' && extra_spacing != 0.0f)
            dx = floorf(dx + extra_spacing);

        prevGlyph = glyphChar;
    }

    const auto drawsort = [](const DrawCommand& a, const DrawCommand& b) -> bool {
        /*
        ** Texture binds are expensive
        ** so we should sort by that first.
        */
        if (a.texture != b.texture)
            return a.texture < b.texture;
        else
            return a.startVertex < b.startVertex;
    };

    std::sort(commands.begin(), commands.end(), drawsort);

    if (dx > maxwidth)
        maxwidth = (int)dx;

    if (info != nullptr)
    {
        info->width  = maxwidth - offset.x;
        info->height = (int)dy +
                       (dx > 0.0f ? floorf(this->GetHeight() * this->GetLineHeight() + 0.5f) : 0) -
                       offset.y;
    }

    return commands;
}

std::vector<Font::DrawCommand> Font::GenerateVerticesFormatted(
    const ColoredCodepoints& text, const Colorf& constantColor, float wrap, AlignMode align,
    std::vector<vertex::GlyphVertex>& vertices, TextInfo* info)
{
    wrap = std::max(wrap, 0.0f);

    uint32_t cacheid = this->textureCacheID;

    std::vector<DrawCommand> drawcommands;
    vertices.reserve(text.codes.size() * 4);

    std::vector<int> widths;
    std::vector<ColoredCodepoints> lines;

    this->GetWrap(text, wrap, lines, &widths);

    float y        = 0.0f;
    float maxwidth = 0.0f;

    for (int i = 0; i < (int)lines.size(); i++)
    {
        const auto& line = lines[i];

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
                float numspaces = (float)std::count(line.codes.begin(), line.codes.end(), ' ');
                if (width < wrap && numspaces >= 1)
                    extraspacing = (wrap - width) / numspaces;
                else
                    extraspacing = 0.0f;
                break;
            }
            case ALIGN_LEFT:
            default:
                break;
        }

        std::vector<DrawCommand> newcommands =
            this->GenerateVertices(line, constantColor, vertices, extraspacing, offset);

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

        y += this->GetHeight() * this->GetLineHeight();
    }

    if (info != nullptr)
    {
        info->width  = (int)maxwidth;
        info->height = (int)y;
    }

    if (cacheid != textureCacheID)
    {
        vertices.clear();
        drawcommands = this->GenerateVerticesFormatted(text, constantColor, wrap, align, vertices);
    }

    return drawcommands;
}

float Font::GetKerning(uint32_t leftGlyph, uint32_t rightGlyph)
{
    uint64_t packed     = ((uint64_t)leftGlyph << 32) | (uint64_t)rightGlyph;
    const auto iterator = this->kerning.find(packed);

    if (iterator != this->kerning.end())
        return iterator->second;

    float kern = rasterizers[0]->GetKerning(leftGlyph, rightGlyph);

    for (const auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(leftGlyph) && rasterizer->HasGlyph(rightGlyph))
        {
            kern = floorf(rasterizer->GetKerning(leftGlyph, rightGlyph) / this->dpiScale + 0.5f);
            break;
        }
    }

    this->kerning[packed] = kern;

    return kern;
}

void Font::Print(Graphics* gfx, const std::vector<Font::ColoredString>& text,
                 const Matrix4& localTransform, const Colorf& color)
{
    ColoredCodepoints codepoints;
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<GlyphVertex> vertices;
    std::vector<DrawCommand> drawCommands = this->GenerateVertices(codepoints, color, vertices);

    this->PrintV(gfx, localTransform, drawCommands, vertices);
}

void Font::Printf(Graphics* gfx, const std::vector<Font::ColoredString>& text, float wrap,
                  Font::AlignMode align, const Matrix4& localTransform, const Colorf& color)
{
    ColoredCodepoints codepoints;
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<GlyphVertex> vertices;
    std::vector<DrawCommand> drawCommands =
        this->GenerateVerticesFormatted(codepoints, color, wrap, align, vertices);

    this->PrintV(gfx, localTransform, drawCommands, vertices);
}

void Font::PrintV(Graphics* gfx, const Matrix4& t, const std::vector<DrawCommand>& drawCommands,
                  const std::vector<GlyphVertex>& vertices)
{
    if (vertices.empty() || drawCommands.empty())
        return;

    Matrix4 m(gfx->GetTransform(), t);

    for (const DrawCommand& cmd : drawCommands)
    {
        vertex::GlyphVertex vertexData[cmd.vertexCount];

        memcpy(vertexData, &vertices[cmd.startVertex], sizeof(GlyphVertex) * cmd.vertexCount);
        m.TransformXY(vertexData, &vertices[cmd.startVertex], cmd.vertexCount);

        std::vector<Vertex> verts = vertex::GenerateTextureFromGlyphs(vertexData, cmd.vertexCount);

        ::deko3d::Instance().RenderTexture(cmd.texture->GetHandle(), verts.data(), cmd.vertexCount);
    }
}

void Font::GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                   std::vector<std::string>& lines, std::vector<int>* linewidths)
{
    ColoredCodepoints cps;
    Font::GetCodepointsFromString(text, cps);

    std::vector<ColoredCodepoints> codepointlines;
    this->GetWrap(cps, wraplimit, codepointlines, linewidths);

    std::string line;

    for (const ColoredCodepoints& codepoints : codepointlines)
    {
        line.clear();
        line.reserve(codepoints.codes.size());

        for (uint32_t codepoint : codepoints.codes)
        {
            char character[5] = { '\0' };
            char* end         = utf8::unchecked::append(codepoint, character);

            line.append(character, end - character);
        }

        lines.push_back(line);
    }
}

void Font::GetWrap(const ColoredCodepoints& codepoints, float wraplimit,
                   std::vector<ColoredCodepoints>& lines, std::vector<int>* linewidths)
{
    // Per-line info.
    float width = 0.0f;

    float widthbeforelastspace = 0.0f;
    float widthoftrailingspace = 0.0f;

    uint32_t prevglyph = 0;

    int lastspaceindex = -1;

    // Keeping the indexed colors "in sync" is a bit tricky, since we split
    // things up and we might skip some glyphs but we don't want to skip any
    // color which starts at those indices.
    Colorf curcolor(1.0f, 1.0f, 1.0f, 1.0f);
    bool addcurcolor = false;
    int curcolori    = -1;
    int endcolori    = (int)codepoints.colors.size() - 1;

    // A wrapped line of text.
    ColoredCodepoints wline;

    int i = 0;
    while (i < (int)codepoints.codes.size())
    {
        uint32_t c = codepoints.codes[i];

        // Determine the current color before doing anything else, to make sure
        // it's still applied to future glyphs even if this one is skipped.
        if (curcolori < endcolori && codepoints.colors[curcolori + 1].index == i)
        {
            curcolor = codepoints.colors[curcolori + 1].color;
            curcolori++;
            addcurcolor = true;
        }

        // Split text at newlines.
        if (c == '\n')
        {
            lines.push_back(wline);

            // Ignore the width of any trailing spaces, for individual lines.
            if (linewidths)
                linewidths->push_back(width - widthoftrailingspace);

            // Make sure the new line keeps any color that was set previously.
            addcurcolor = true;

            width = widthbeforelastspace = widthoftrailingspace = 0.0f;
            prevglyph                                           = 0; // Reset kerning information.
            lastspaceindex                                      = -1;
            wline.codes.clear();
            wline.colors.clear();
            i++;

            continue;
        }

        // Ignore carriage returns
        if (c == '\r')
        {
            i++;
            continue;
        }

        const Glyph& g  = this->FindGlyph(c);
        float charwidth = g.spacing + this->GetKerning(prevglyph, c);
        float newwidth  = width + charwidth;

        // Wrap the line if it exceeds the wrap limit. Don't wrap yet if we're
        // processing a newline character, though.
        if (c != ' ' && newwidth > wraplimit)
        {
            // If this is the first character in the line and it exceeds the
            // limit, skip it completely.
            if (wline.codes.empty())
                i++;
            else if (lastspaceindex != -1)
            {
                // 'Rewind' to the last seen space, if the line has one.
                // FIXME: This could be more efficient...
                while (!wline.codes.empty() && wline.codes.back() != ' ')
                    wline.codes.pop_back();

                while (!wline.colors.empty() &&
                       wline.colors.back().index >= (int)wline.codes.size())
                    wline.colors.pop_back();

                // Also 'rewind' to the color that the last character is using.
                for (int colori = curcolori; colori >= 0; colori--)
                {
                    if (codepoints.colors[colori].index <= lastspaceindex)
                    {
                        curcolor  = codepoints.colors[colori].color;
                        curcolori = colori;
                        break;
                    }
                }

                // Ignore the width of trailing spaces in wrapped lines.
                width = widthbeforelastspace;

                i = lastspaceindex;
                i++; // Start the next line after the space.
            }

            lines.push_back(wline);

            if (linewidths)
                linewidths->push_back(width);

            addcurcolor = true;

            prevglyph = 0;
            width = widthbeforelastspace = widthoftrailingspace = 0.0f;
            wline.codes.clear();
            wline.colors.clear();
            lastspaceindex = -1;

            continue;
        }

        if (prevglyph != ' ' && c == ' ')
            widthbeforelastspace = width;

        width     = newwidth;
        prevglyph = c;

        if (addcurcolor)
        {
            wline.colors.push_back({ curcolor, (int)wline.codes.size() });
            addcurcolor = false;
        }

        wline.codes.push_back(c);

        // Keep track of the last seen space, so we can "rewind" to it when
        // wrapping.
        if (c == ' ')
        {
            lastspaceindex = i;
            widthoftrailingspace += charwidth;
        }
        else if (c != '\n')
            widthoftrailingspace = 0.0f;

        i++;
    }

    // Push the last line.
    if (!wline.codes.empty())
    {
        lines.push_back(wline);

        // Ignore the width of any trailing spaces, for individual lines.
        if (linewidths)
            linewidths->push_back(width - widthoftrailingspace);
    }
}

float Font::GetAscent() const
{
    return floorf(this->rasterizers[0]->GetAscent() / this->dpiScale + 0.5f);
}

float Font::GetDescent() const
{
    return floorf(this->rasterizers[0]->GetDescent() / this->dpiScale + 0.5f);
}

float Font::GetHeight() const
{
    return floorf(this->height / this->dpiScale + 0.5f);
}

float Font::GetBaseline() const
{
    float ascent = this->GetAscent();

    if (ascent != 0.0f)
        return ascent;
    else if (this->rasterizers[0]->GetDataType() == love::Rasterizer::DATA_TRUETYPE)
        return floorf(this->GetHeight() / 1.25f + 0.5f);
    else
        return 0.0f;
}

int Font::GetWidth(uint32_t prevGlyph, uint32_t current)
{
    const Glyph& g = this->FindGlyph(current);
    return g.spacing + this->GetKerning(prevGlyph, current);
}

void Font::SetFallbacks(const std::vector<Font*>& fallbacks)
{
    for (const Font* font : fallbacks)
    {
        if (font->rasterizers[0]->GetDataType() != this->rasterizers[0]->GetDataType())
            throw love::Exception("Font fallbacks must be of the same font type.");
    }

    this->rasterizers.resize(1);

    // NOTE: this won't invalidate already-rasterized glyphs.
    for (const Font* font : fallbacks)
        this->rasterizers.push_back(font->rasterizers[0]);
}

float Font::GetKerning(const std::string& leftChar, const std::string& rightChar)
{
    uint32_t left  = 0;
    uint32_t right = 0;

    try
    {
        left  = utf8::peek_next(leftChar.begin(), leftChar.end());
        right = utf8::peek_next(rightChar.begin(), rightChar.end());
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return this->GetKerning(left, right);
}

// clang-format off
constexpr StringMap<love::Font::SystemFontType, love::Font::MAX_SYSFONTS>::Entry sharedFontEntries[] =
{
    { "standard",                    Font::SystemFontType::TYPE_STANDARD               },
    { "chinese simplified",          Font::SystemFontType::TYPE_CHINESE_SIMPLIFIED     },
    { "chinese traditional",         Font::SystemFontType::TYPE_CHINESE_TRADITIONAL    },
    { "extended chinese simplified", Font::SystemFontType::TYPE_CHINESE_SIMPLIFIED_EXT },
    { "korean",                      Font::SystemFontType::TYPE_KOREAN                 },
    { "nintendo extended",           Font::SystemFontType::TYPE_NINTENDO_EXTENDED      }
};

constinit const StringMap<Font::SystemFontType, love::Font::MAX_SYSFONTS> love::common::Font::sharedFonts(sharedFontEntries);
// clang-format on
