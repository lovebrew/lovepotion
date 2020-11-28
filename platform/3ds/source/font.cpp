#include "common/runtime.h"
#include "objects/font/font.h"

#include "modules/graphics/graphics.h"

using namespace love;

Font::Font(const Rasterizer & rasterizer, const Texture::Filter & filter) : rasterizer(rasterizer)
{
    if (rasterizer.data != nullptr)
        this->rasterizer.font = C2D_FontLoadFromMem(rasterizer.data->GetData(), rasterizer.data->GetSize());

    this->rasterizer.buffer = C2D_TextBufNew(Font::FONT_BUFFER_SIZE);

    this->lineHeight = 1.0f;
}

Font::~Font()
{
    C2D_TextBufDelete(this->rasterizer.buffer);
    C2D_FontFree(this->rasterizer.font);
}

common::Font * common::Font::GetSystemFontByType(int size, Font::SystemFontType type, const Texture::Filter & filter)
{
    const Rasterizer rasterizer =
    {
        .size = size,
        .data = nullptr,

        .font = C2D_FontLoadSystem((CFG_Region)type),
        .buffer = nullptr,
    };

    return new love::Font(rasterizer, filter);
}

void Font::Print(Graphics * gfx, const std::vector<ColoredString> & text,
                 const Matrix4 & localTransform, const Colorf & color)
{
    C2D_Text citroText;

    std::string result = std::accumulate(text.begin(), text.end(), std::string{},
                         [](const std::string & s1, const ColoredString & piece) { return s1 + piece.string; });

    C2D_TextFontParse(&citroText, this->rasterizer.font, this->rasterizer.buffer, result.c_str());
    C2D_TextOptimize(&citroText);

    Matrix4 t(gfx->GetTransform(), localTransform);
    C2D_ViewRestore(&t.GetElements());

    u32 renderColorf = C2D_Color32f(color.r, color.g, color.b, color.a);
    C2D_DrawText(&citroText, C2D_WithColor, 0, 0, Graphics::CURRENT_DEPTH, this->GetScale(), this->GetScale(), renderColorf);
}

void Font::Printf(Graphics * gfx, const std::vector<ColoredString> & text, float wrap, AlignMode align,
                  const Matrix4 & localTransform, const Colorf & color)
{
    C2D_Text citroText;
    u32 alignMode = C2D_AlignLeft;

    switch (align)
    {
        case Font::ALIGN_CENTER:
            alignMode = C2D_AlignCenter;
        case Font::ALIGN_RIGHT:
            alignMode = C2D_AlignRight;
        case Font::ALIGN_JUSTIFY:
            alignMode = C2D_AlignJustified | C2D_WordWrap;
        default:
            break;
    }

    std::string result = std::accumulate(text.begin(), text.end(), std::string{},
                         [](const std::string & s1, const ColoredString & piece) { return s1 + piece.string; });

    C2D_TextFontParse(&citroText, this->rasterizer.font, this->rasterizer.buffer, result.c_str());
    C2D_TextOptimize(&citroText);

    Matrix4 t(gfx->GetTransform(), localTransform);
    C2D_ViewRestore(&t.GetElements());

    u32 renderColorf = C2D_Color32f(color.r, color.g, color.b, color.a);
    C2D_DrawText(&citroText, C2D_WithColor | alignMode, 0, 0, Graphics::CURRENT_DEPTH, this->GetScale(), this->GetScale(), renderColorf, wrap);
}

void Font::ClearBuffer()
{
    C2D_TextBufClear(this->rasterizer.buffer);
}

int Font::GetWidth(uint32_t /* prevGlyph */, uint32_t current)
{
    auto found = this->glyphWidths.find(current);
    if (found != this->glyphWidths.end())
        return found->second;

    fontGlyphPos_s out;
    int glyphIndex = C2D_FontGlyphIndexFromCodePoint(this->rasterizer.font, current);
    C2D_FontCalcGlyphPos(this->rasterizer.font, &out, glyphIndex, 0, this->GetScale(), this->GetScale());

    this->glyphWidths.emplace(current, out.xAdvance);
    return out.xAdvance;
}

float Font::GetScale() const
{
    return (this->rasterizer.size / 30.0f);
}

float Font::GetHeight() const
{
    return 30 * this->GetScale();
}

StringMap<Font::SystemFontType, Font::MAX_SYSFONTS>::Entry common::Font::sharedFontEntries[] =
{
    { "standard",  SystemFontType::TYPE_STANDARD  },
    { "chinese",   SystemFontType::TYPE_CHINESE   },
    { "taiwanese", SystemFontType::TYPE_TAIWANESE },
    { "korean",    SystemFontType::TYPE_KOREAN    },
};

StringMap<Font::SystemFontType, Font::MAX_SYSFONTS> common::Font::sharedFonts(Font::sharedFontEntries, sizeof(Font::sharedFontEntries));
