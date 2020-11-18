#include "common/runtime.h"
#include "objects/font/font.h"

#include "modules/font/fontmodule.h"
#include "modules/graphics/graphics.h"
#include "common/matrix.h"

using namespace love;

#define FONT_MODULE() (Module::GetInstance<FontModule>(Module::M_FONT))

Font::Font(Data * data, float size) : size(size)
{
    // this->font = TTF_OpenFontRW(SDL_RWFromMem(data->GetData(), data->GetSize()), 1, size);
}

Font::Font(Font::SystemFontType type, float size) : size(size)
{
    PlFontData data;
    plGetSharedFontByType(&data, (PlSharedFontType)type);

    Rasterizer * rasterizer = FONT_MODULE()->NewTrueTypeRasterizer(data.address, data.size,
                                                                   size, TrueTypeRasterizer::HINTING_NORMAL);
    this->font.Load(rasterizer, Texture::defaultFilter);
}

Font::Font(float size) : size(size)
{
    PlFontData data;
    plGetSharedFontByType(&data, (PlSharedFontType)SystemFontType::TYPE_STANDARD);

    Rasterizer * rasterizer = FONT_MODULE()->NewTrueTypeRasterizer(data.address, data.size,
                                                                   size, TrueTypeRasterizer::HINTING_NORMAL);
    this->font.Load(rasterizer, Texture::defaultFilter);
}

void Font::GetCodepointsFromString(const std::string & string, Codepoints & codepoints)
{
    codepoints.reserve(string.size());

    for (size_t i = 0; i < string.size(); i++)
    {
        uint32_t codepoint;
        auto bytes = decode_utf8(&codepoint, (uint8_t *)&string[i]);

        if (bytes < 0)
        {
            bytes = 1; // skip the invalid sequence
            codepoint = 0xFFFD;
        }

        LOG("We got codepoint: %zu", codepoint);
        codepoints.push_back(codepoint);
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
{}

void Font::GenerateVertices(const ColoredCodepoints & codepoints, const Colorf & constantColor, std::vector<vertex::Vertex> & glyphVertices,
                            float extra_spacing, Vector2 offset, TextInfo * info)
{
    /*
    ** Spacing counter and
    ** newline handling.
    */
    float dx = offset.x;
    float dy = offset.y;

    float heightoffset = 0.0f;

    LOG("Get Rasterizer Data Type");
    if (this->font.GetRasterizers()[0]->GetDataType() == Rasterizer::DATA_TRUETYPE)
        heightoffset = this->font.GetBaseline();

    int maxwidth = 0;

    // Pre-allocate space for the maximum possible number of vertices.
    LOG("Preallocate vertex stuff");
    size_t vertstartsize = glyphVertices.size();
    glyphVertices.reserve(vertstartsize + codepoints.codes.size() * 4);

    uint32_t prevGlyph = 0;

    Colorf linearConstant = Graphics::GammaCorrectColor(constantColor);
    Colorf currentColor = constantColor;

    int currentColorIndex = -1;
    int numColors = (int)codepoints.colors.size();
    LOG("numColors is %d", numColors);

    for (int i = 0; i < (int)codepoints.codes.size(); i++)
    {
        uint32_t glyph = codepoints.codes[i];
        LOG("Glyph %zu", glyph);

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
                maxwidth = (int) dx;

            // Wrap newline, but do not print it.
            dy += floorf(this->font.GetHeight() * this->GetLineHeight() + 0.5f);
            dx = offset.x;

            prevGlyph = 0;

            continue;
        }

        // Ignore carriage returns
        if (glyph == '\r')
            continue;

        const CFont::Glyph & glyphData = this->font.FindGlyph(glyph);

        // Add kerning to the current horizontal offset.
        LOG("Getting Kerning for Glyph!");
        dx += this->font.GetKerning(prevGlyph, glyph);

        // Copy the vertices and set their colors and relative positions.
        // for (int j = 0; j < 4; j++)
        // {
        //     glyphVertices.push_back(glyphData.vertices[j]);
        //     glyphVertices.back().position[0] += dx;
        //     glyphVertices.back().position[1] += dy + heightoffset;

        //     currentColor.CopyTo(glyphVertices.back().color);
        // }

        dx += glyphData.spacing;

        // Account for extra spacing given to space characters.
        if (glyph == ' ' && extra_spacing != 0.0f)
            dx = floorf(dx + extra_spacing);

        prevGlyph = glyph;
    }

    if (dx > maxwidth)
        maxwidth = (int)dx;

    if (info != nullptr)
    {
        info->width = maxwidth - offset.x;
        info->height = (int) dy + (dx > 0.0f ? floorf(this->font.GetHeight() * this->GetLineHeight() + 0.5f) : 0) - offset.y;
    }
}

void Font::Print(Graphics * gfx, const std::vector<ColoredString> & text,
                 float limit, const Colorf & color, AlignMode align,
                 const Matrix4 & localTransform)
{
    ColoredCodepoints codepoints;
    LOG("GetCodepointsFromString");
    GetCodepointsFromString(text, codepoints);
    LOG("Finished, Generating Vertices..");
    std::vector<vertex::Vertex> vertices;
    this->GenerateVertices(codepoints, color, vertices);
    LOG("Success!");
    this->PrintV(gfx, localTransform, vertices);
}

void Font::PrintV(Graphics * gfx, const Matrix4 & t, const std::vector<vertex::Vertex> & vertices)
{
    if (vertices.empty())
        return;

    Matrix4 m(gfx->GetTransform(), t);
}

float Font::GetSize()
{
    return this->size;
}

float Font::GetHeight()
{
    return this->font.GetHeight();
}

float Font::_GetGlyphWidth(u16 glyph)
{}

float Font::GetWidth(const char * text)
{}

void Font::ClearBuffer()
{}

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM>::Entry Font::sharedFontEntries[] =
{
    { "standard",                    TYPE_STANDARD               },
    { "chinese simplified",          TYPE_CHINESE_SIMPLIFIED     },
    { "chinese traditional",         TYPE_CHINESE_TRADITIONAL    },
    { "extended chinese simplified", TYPE_CHINESE_SIMPLIFIED_EXT },
    { "korean",                      TYPE_KOREAN                 },
    { "nintendo extended",           TYPE_NINTENDO_EXTENDED      }
};

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM> Font::sharedFonts(Font::sharedFontEntries, sizeof(Font::sharedFontEntries));
