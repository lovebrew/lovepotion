#include "common/runtime.h"
#include "objects/font/font.h"

#include "modules/font/fontmodule.h"
#include "modules/graphics/graphics.h"
#include "deko3d/graphics.h"
#include "common/matrix.h"

#include "deko3d/deko.h"
#include "utf8/utf8.h"

using namespace vertex;

using namespace love;

#define FONT_MODULE() (Module::GetInstance<FontModule>(Module::M_FONT))

Font::Font(Rasterizer * r, const Texture::Filter & filter) : rasterizers({r}),
                                                             height(r->GetHeight()),
                                                             lineHeight(1),
                                                             textureWidth(128),
                                                             textureHeight(128),
                                                             useSpacesAsTab(false),
                                                             textureCacheID(0),
                                                             dpiScale(r->GetDPIScale())
{
    while (true)
    {
        if ((r->GetHeight() * 0.8) * r->GetHeight() * 30 <= textureWidth * textureHeight)
            break;

        TextureSize nextsize = this->GetNextTextureSize();

        if (nextsize.width <= textureWidth && nextsize.height <= textureHeight)
            break;

        textureWidth = nextsize.width;
        textureHeight = nextsize.height;
    }

    /* No tab character in the Rasterizer. */
    if (!r->HasGlyph(9))
        this->useSpacesAsTab = true;

    this->textureCacheID++;

    this->glyphs.clear();
    this->images.clear();

    this->CreateTexture();
}

Font::Font(SystemFontType, float)
{}

Font::Font(float)
{}

Font::Font(Data * data, float)
{}

void Font::ClearBuffer()
{}

float Font::GetWidth(const char *)
{ return 0.0f; }

Font::TextureSize Font::GetNextTextureSize() const
{
    TextureSize size = {this->textureWidth, this->textureHeight};

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
    TextureSize size = {this->textureWidth, this->textureHeight};
    TextureSize nextSize = this->GetNextTextureSize();
    bool recreatetexture = false;

    auto gfx = Module::GetInstance<deko3d::Graphics>(Module::M_GRAPHICS);

    love::Image * texture = nullptr;

    // If we have an existing texture already, we'll try replacing it with a
    // larger-sized one rather than creating a second one. Having a single
    // texture reduces texture switches and draw calls when rendering.
    if ((nextSize.width > size.width || nextSize.height > size.height) && !images.empty())
    {
        recreatetexture = true;
        size = nextSize;
        images.pop_back();
    }

    texture = gfx->NewImage(love::Texture::TEXTURE_2D, size.width, size.height);

    this->images.emplace_back(texture, Acquire::NORETAIN);

    this->textureWidth  = size.width;
    this->textureHeight = size.height;

    this->rowHeight = this->textureX = this->textureY = this->TEXTURE_PADDING;

    if (recreatetexture)
    {
        textureCacheID++;

        std::vector<uint32_t> glyphsToAdd;

        for (const auto & glyphPair : this->glyphs)
            glyphsToAdd.push_back(glyphPair.first);

        glyphs.clear();

        for (uint32_t glyph : glyphsToAdd)
            this->AddGlyph(glyph);
    }
}

love::GlyphData * Font::GetRasterizerGlyphData(uint32_t glyph)
{
    // Use spaces for the tab 'glyph'.
    if (glyph == 9 && useSpacesAsTab)
    {
        love::GlyphData * spacegd = this->rasterizers[0]->GetGlyphData(32);

        love::GlyphData::GlyphMetrics gm = {};

        gm.advance  = spacegd->GetAdvance() * SPACES_PER_TAB;
        gm.bearingX = spacegd->GetBearingX();
        gm.bearingY = spacegd->GetBearingY();

        spacegd->Release();

        return new love::GlyphData(glyph, gm);
    }

    for (const love::StrongReference<love::Rasterizer> & r : rasterizers)
    {
        if (r->HasGlyph(glyph))
            return r->GetGlyphData(glyph);
    }

    return rasterizers[0]->GetGlyphData(glyph);
}

const Font::Glyph & Font::FindGlyph(uint32_t glyph)
{
    const auto it = this->glyphs.find(glyph);

    if (it != this->glyphs.end())
        return it->second;

    return this->AddGlyph(glyph);
}

static inline uint16_t norm16(double n)
{
    return uint16_t(n * 0xFFFF);
}

const Font::Glyph & Font::AddGlyph(uint32_t glyph)
{
    love::StrongReference<love::GlyphData> gd(this->GetRasterizerGlyphData(glyph), Acquire::NORETAIN);

    int width  = gd->GetWidth();
    int height = gd->GetHeight();

    if (width + this->TEXTURE_PADDING * 2 < this->textureWidth && height + this->TEXTURE_PADDING * 2 < this->textureHeight)
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
            // Totally out of space - new texture!
            this->CreateTexture();

            // Makes sure the above code for checking if the glyph can fit at
            // the current position in the texture is run again for this glyph.
            return this->AddGlyph(glyph);
        }
    }

    Glyph g;

    g.texture = 0;
    g.spacing = floorf(gd->GetAdvance() / this->dpiScale + 0.5f);
    std::fill_n(g.vertices, 4, GlyphVertex{});

    // Don't waste space on empty glyphs
    if (width > 0 && height > 0)
    {
        Image * image = images.back();
        g.texture = image;

        Rect rect = {this->textureX, this->textureY, gd->GetWidth(), gd->GetHeight()};
        image->ReplacePixels(gd->GetData(), gd->GetSize(), rect);

        double tX     = (double) textureX,     tY      = (double) textureY;
        double tWidth = (double) textureWidth, tHeight = (double) textureHeight;

        Colorf c(1.0f, 1.0f, 1.0f, 1.0f);

        // Extrude the quad borders by 1 pixel. We have an extra pixel of
        // transparent padding in the texture atlas, so the quad extrusion will
        // add some antialiasing at the edges of the quad.
        int o = 1;

        // 0---3
        // |   |
        // 1---2
        const GlyphVertex verts[4] =
        {
            { float(-o),                    float(-o),                     norm16((tX - o)/ tWidth),          norm16((tY - o) / tHeight),          c },
            { float(-o),                    (height + o) / this->dpiScale, norm16((tX - o) / tWidth),         norm16((tY + height + o) / tHeight), c },
            { (width + o) / this->dpiScale, (height + o) / this->dpiScale, norm16((tX + width + o) / tWidth), norm16((tY + height + o) / tHeight), c },
            { (width + o) / this->dpiScale, float(-o),                     norm16((tX + width + o) / tWidth), norm16((tY - o)          / tHeight), c },
        };

        // Copy vertex data to the glyph
        // and set proper bearing.
        for (int i = 0; i < 4; i++)
        {
            g.vertices[i] = verts[i];

            g.vertices[i].x += gd->GetBearingX() / this->dpiScale;
            g.vertices[i].y -= gd->GetBearingY() / this->dpiScale;
        }

        this->textureX  += width + this->TEXTURE_PADDING;
        this->rowHeight =  std::max(this->rowHeight, height + this->TEXTURE_PADDING);
    }

    this->glyphs.emplace(glyph, g);
    return this->glyphs[glyph];
}

void Font::GetCodepointsFromString(const std::string & text, Codepoints & codepoints)
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
    catch (utf8::exception &e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }
}

void Font::GetCodepointsFromString(const std::vector<ColoredString> & strings, ColoredCodepoints & codepoints)
{
    if (strings.empty())
        return;

    codepoints.codes.reserve(strings[0].string.size());

    for (const ColoredString & coloredString : strings)
    {
        if (coloredString.string.size() == 0)
            continue;

        IndexedColor iColor = {coloredString.color, (int)codepoints.codes.size()};
        codepoints.colors.push_back(iColor);

        GetCodepointsFromString(coloredString.string, codepoints.codes);
    }

    if (codepoints.colors.size() == 1)
    {
        IndexedColor iColor = codepoints.colors[0];

        if (iColor.index == 0 && iColor.color == Colorf(1.0f, 1.0f, 1.0f, 1.0f))
            codepoints.colors.pop_back();
    }
}

Font::~Font()
{
    this->glyphs.clear();
    this->images.clear();
}

std::vector<Font::DrawCommand> Font::GenerateVertices(const ColoredCodepoints & codepoints, const Colorf & constantColor,
                                                      std::vector<GlyphVertex> & glyphVertices, float extra_spacing,
                                                      Vector2 offset, TextInfo * info)
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
    Colorf currentColor = constantColor;

    int currentColorIndex = -1;
    int numColors = (int)codepoints.colors.size();

    for (int i = 0; i < (int)codepoints.codes.size(); i++)
    {
        uint32_t glyph = codepoints.codes[i];

        if (currentColorIndex + 1 < numColors && codepoints.colors[currentColorIndex + 1].index == i)
        {
            Colorf c = codepoints.colors[++numColors].color;

            c.r = std::min(std::max(c.r, 0.0f), 1.0f);
            c.g = std::min(std::max(c.g, 0.0f), 1.0f);
            c.b = std::min(std::max(c.b, 0.0f), 1.0f);
            c.a = std::min(std::max(c.a, 0.0f), 1.0f);

            Graphics::GammaCorrectColor(c);
            c *= linearConstant;
            Graphics::UnGammaCorrectColor(c);
        }

        if (glyph == '\n')
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
        if (glyph == '\r')
            continue;

        uint32_t cacheID = textureCacheID;

        const Glyph & glyphData = this->FindGlyph(glyph);

        /*
        ** If findGlyph invalidates the texture cache
        ** restart the loop.
        */
        if (cacheID != this->textureCacheID)
        {
            i = -1; // The next iteration will increment this to 0.
            maxwidth = 0;

            dx = offset.x;
            dy = offset.y;

            commands.clear();

            glyphVertices.resize(vertstartsize);
            prevGlyph = 0;

            currentColorIndex = -1;
            currentColor = constantColor;

            continue;
        }

        /* Add kerning to the current horizontal offset. */
        dx += this->GetKerning(prevGlyph, glyph);

        if (glyphData.texture != nullptr)
        {
            /*
            ** Copy the vertices and set their colors
            ** and relative positions.
            */
            for (int j = 0; j < 4; j++)
            {
                glyphVertices.emplace_back(glyphData.vertices[j]);
                glyphVertices.back().x += dx;
                glyphVertices.back().y += dy + heightoffset;

                glyphVertices.back().color = currentColor;
            }

            // Check if glyph texture has changed since the last iteration.
            if (commands.empty() || commands.back().texture != glyphData.texture)
            {
                /*
                ** Add a new draw command
                ** if the texture has changed.
                */
                DrawCommand cmd;
                cmd.startVertex = (int)glyphVertices.size() - 4;
                cmd.vertexCount = 0;

                cmd.texture = glyphData.texture;
                commands.push_back(cmd);
            }

            commands.back().vertexCount += 4;
        }

        dx += glyphData.spacing;

        // Account for extra spacing given to space characters.
        if (glyph == ' ' && extra_spacing != 0.0f)
            dx = floorf(dx + extra_spacing);

        prevGlyph = glyph;
    }

    const auto drawsort = [](const DrawCommand & a, const DrawCommand & b) -> bool
    {
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
        info->width = maxwidth - offset.x;
        info->height = (int)dy + (dx > 0.0f ? floorf(this->GetHeight() * this->GetLineHeight() + 0.5f) : 0) - offset.y;
    }

    return commands;
}

float Font::GetLineHeight() const
{
    return this->lineHeight;
}

std::vector<Font::DrawCommand> Font::GenerateVerticesFormatted(const ColoredCodepoints & text, const Colorf & constantColor, float wrap,
                                                               AlignMode align, std::vector<vertex::GlyphVertex> & vertices, TextInfo * info)
{}

float Font::GetKerning(uint32_t leftGlyph, uint32_t rightGlyph)
{
    uint64_t packed = ((uint64_t)leftGlyph << 32) | (uint64_t)rightGlyph;
    const auto iterator = this->kerning.find(packed);

    if (iterator != this->kerning.end())
        return iterator->second;

    float kern = rasterizers[0]->GetKerning(leftGlyph, rightGlyph);

    for (const auto & rasterizer : this->rasterizers)
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

void Font::Print(Graphics * gfx, const std::vector<ColoredString> & text,
                 const Matrix4 & localTransform, const Colorf & color)
{
    ColoredCodepoints codepoints;
    GetCodepointsFromString(text, codepoints);

    std::vector<GlyphVertex> vertices;
    std::vector<DrawCommand> drawCommands = this->GenerateVertices(codepoints, color, vertices);

    this->PrintV(gfx, localTransform, drawCommands, vertices);
}

void Font::Printf(Graphics * gfx, const std::vector<ColoredString> & text, float wrap,
                  AlignMode align, const Matrix4 & localTransform, const Colorf & color)
{}

void Font::PrintV(Graphics * gfx, const Matrix4 & t, const std::vector<DrawCommand> & drawCommands,
                  const std::vector<GlyphVertex> & vertices)
{
    if (vertices.empty() || drawCommands.empty())
        return;

    Matrix4 m(gfx->GetTransform(), t);

    for (const DrawCommand & cmd : drawCommands)
    {
        size_t vertexCount = cmd.vertexCount;

        std::vector<GlyphVertex> vertexData (vertexCount);

        memcpy(vertexData.data (), &vertices[cmd.startVertex], sizeof(GlyphVertex) * vertexCount);
        m.TransformXY(vertexData.data (), &vertices[cmd.startVertex], vertexCount);

        std::vector<Vertex> verts = vertex::GenerateTextureFromGlyphs(vertexData.data(), vertexCount);
        dk3d.RenderTexture(cmd.texture->GetHandle(), verts.data(), vertexCount * sizeof(*verts.data()), vertexCount);
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

int Font::GetWidth(char character)
{
    const Glyph & g = this->FindGlyph(character);
    return g.spacing;
}

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM>::Entry love::Font::sharedFontEntries[] =
{
    { "standard",                    TYPE_STANDARD               },
    { "chinese simplified",          TYPE_CHINESE_SIMPLIFIED     },
    { "chinese traditional",         TYPE_CHINESE_TRADITIONAL    },
    { "extended chinese simplified", TYPE_CHINESE_SIMPLIFIED_EXT },
    { "korean",                      TYPE_KOREAN                 },
    { "nintendo extended",           TYPE_NINTENDO_EXTENDED      }
};

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM> love::Font::sharedFonts(Font::sharedFontEntries, sizeof(Font::sharedFontEntries));
