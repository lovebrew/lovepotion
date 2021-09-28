#include "objects/font/font.h"
#include "modules/graphics/graphics.h"

#include "citro2d/citro.h"

#include <numeric>

using namespace love;

Font::Font(Rasterizer* rasterizer, const Texture::Filter& filter) :
    common::Font(filter),
    rasterizer(rasterizer),
    buffer(C2D_TextBufNew(Font::FONT_BUFFER_SIZE))
{
    this->dpiScale = rasterizer->GetDPIScale();
    this->height   = rasterizer->GetHeight();

    this->lineHeight = rasterizer->GetLineHeight();

    ::citro2d::GPUFilter gpuFilter = ::citro2d::GetCitroFilterMode(filter);
    C2D_FontSetFilter(this->GetFont(), gpuFilter.mag, gpuFilter.min);
}

Font::~Font()
{
    C2D_TextBufClear(this->buffer);
    C2D_TextBufDelete(this->buffer);
}

const C2D_Font Font::GetFont()
{
    auto r = static_cast<BCFNTRasterizer*>(this->rasterizer.Get());
    return r->GetFont();
}

float Font::GetScale() const
{
    auto r = static_cast<BCFNTRasterizer*>(this->rasterizer.Get());

    return r->GetScale();
}

float Font::GetDPIScale() const
{
    return this->dpiScale;
}

void Font::SetFilter(const Texture::Filter& filter)
{
    this->filter = filter;
}

float Font::GetAscent() const
{
    return floorf(this->rasterizer->GetAscent() / this->dpiScale + 0.5f);
}

float Font::GetDescent() const
{
    return floorf(this->rasterizer->GetDescent() / this->dpiScale + 0.5f);
}

bool Font::HasGlyph(uint32_t glyph) const
{
    return this->rasterizer->HasGlyph(glyph);
}

float Font::GetKerning(const std::string&, const std::string&)
{
    return 0.0f;
}

float Font::GetKerning(uint32_t leftGlyph, uint32_t rightGlyph)
{
    return 0.0f;
}

void Font::SetFallbacks(const std::vector<Font*>& fallbacks)
{}

float Font::GetBaseline() const
{
    float ascent = this->GetAscent();

    if (ascent != 0.0f)
        return ascent;
    else if (this->rasterizer->GetDataType() == love::Rasterizer::DATA_BCFNT)
        return floorf(this->GetHeight() / 1.0f);
    else
        return 0.0f;
}

void Font::GetWrap(const std::vector<Font::ColoredString>& strings, float wrapLimit,
                   std::vector<std::string>& lines, std::vector<int>* lineWidths)
{}

void Font::Print(Graphics* gfx, const std::vector<ColoredString>& text,
                 const Matrix4& localTransform, const Colorf& color)
{
    C2D_Text citroText;

    std::string result = std::accumulate(
        text.begin(), text.end(), std::string {},
        [](const std::string& s1, const ColoredString& piece) { return s1 + piece.string; });

    C2D_TextFontParse(&citroText, this->GetFont(), this->buffer, result.c_str());
    C2D_TextOptimize(&citroText);

    Matrix4 t(gfx->GetTransform(), localTransform);
    C2D_ViewRestore(&t.GetElements());

    u32 renderColorf = C2D_Color32f(color.r, color.g, color.b, color.a);
    C2D_DrawText(&citroText, C2D_WithColor, 0, 0, Graphics::CURRENT_DEPTH, this->GetScale(),
                 this->GetScale(), renderColorf);

    C2D_TextBufClear(this->buffer);
}

void Font::Printf(Graphics* gfx, const std::vector<ColoredString>& text, float wrap,
                  AlignMode align, const Matrix4& localTransform, const Colorf& color)
{
    C2D_Text citroText;

    u32 alignMode = C2D_WordWrap;
    float offset  = 0.0f;

    switch (align)
    {
        case Font::ALIGN_LEFT:
        default:
            alignMode |= C2D_AlignLeft;
            break;
        case Font::ALIGN_CENTER:
            alignMode |= C2D_AlignCenter;
            offset += wrap / 2;
            break;
        case Font::ALIGN_RIGHT:
            alignMode |= C2D_AlignRight;
            break;
        case Font::ALIGN_JUSTIFY:
            alignMode |= C2D_AlignJustified;
            break;
    }

    std::string result = std::accumulate(
        text.begin(), text.end(), std::string {},
        [](const std::string& s1, const ColoredString& piece) { return s1 + piece.string; });

    C2D_TextFontParse(&citroText, this->GetFont(), this->buffer, result.c_str());
    C2D_TextOptimize(&citroText);

    Matrix4 t(gfx->GetTransform(), localTransform);
    C2D_ViewRestore(&t.GetElements());

    u32 renderColorf = C2D_Color32f(color.r, color.g, color.b, color.a);
    C2D_DrawText(&citroText, C2D_WithColor | alignMode, offset, 0, Graphics::CURRENT_DEPTH,
                 this->GetScale(), this->GetScale(), renderColorf, wrap);

    C2D_TextBufClear(this->buffer);
}

int Font::GetWidth(uint32_t /* prevGlyph */, uint32_t current)
{
    auto found = this->glyphWidths.find(current);

    if (found != this->glyphWidths.end())
        return found->second;

    GlyphData* glyphData = this->rasterizer->GetGlyphData(current);

    this->glyphWidths[current] = glyphData->GetAdvance();

    return this->glyphWidths[current];
}

float Font::GetHeight() const
{
    return 30 * this->GetScale();
}

// clang-format off
constexpr StringMap<Font::SystemFontType, Font::MAX_SYSFONTS>::Entry sharedFontEntries[] =
{
    { "standard",  Font::SystemFontType::TYPE_STANDARD  },
    { "chinese",   Font::SystemFontType::TYPE_CHINESE   },
    { "taiwanese", Font::SystemFontType::TYPE_TAIWANESE },
    { "korean",    Font::SystemFontType::TYPE_KOREAN    }
};

constinit const StringMap<Font::SystemFontType, Font::MAX_SYSFONTS> common::Font::sharedFonts(sharedFontEntries);
// clang-format on
