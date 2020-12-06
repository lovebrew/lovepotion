#include "common/runtime.h"
#include "objects/text/text.h"

#include "modules/graphics/graphics.h"

using namespace love;

Text::Text(Font * font, const std::vector<Font::ColoredString> & text) : common::Text(font, text)
{
    this->buffer = C2D_TextBufNew(Font::FONT_BUFFER_SIZE);
}

Text::~Text()
{
    C2D_TextBufDelete(this->buffer);
}

void Text::SetFont(Font * font)
{
    this->font.Set(font);
}

std::string Text::GetString(const std::vector<Font::ColoredString> & text)
{
    std::string result = std::accumulate(text.begin(), text.end(), std::string{},
                         [](std::string & s1, const Font::ColoredString & piece) { return s1 + piece.string; });

    return result;
}

/*
** Text objects on 3DS can't have
** a lot of .. text? Like using add(f)
** So just return the entire string width
*/
int Text::GetWidth(int index) const
{
    int width = this->font->GetWidth(this->textCache);
    return width;
}

/*
** Text objects on 3DS can't have
** a lot of .. text? Like using add(f)
** So just return the entire string height
*/
int Text::GetHeight(int index) const
{
    return this->font->GetHeight();
}

void Text::Set(const std::vector<Font::ColoredString> & text)
{
    return this->Set(text, -1.0f, Font::ALIGN_MAX_ENUM);
}

void Text::Set(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align)
{
    if (text.empty() || text.size() == 1 && text[0].string.empty())
        return this->Clear();

    this->textCache = this->GetString(text);

    C2D_TextFontParse(&this->text, this->font->GetFont(), this->buffer, this->textCache.c_str());
    C2D_TextOptimize(&this->text);
}

int Text::Add(const std::vector<Font::ColoredString> & text, const Matrix4 & localTransform)
{
    return this->Addf(text, -1.0f, Font::ALIGN_MAX_ENUM, localTransform);
}

/*
** Galaxy brain move here:
** always return 0 because we don't
** use a vector of data for strings on 3DS
*/
int Text::Addf(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align, const Matrix4 & localTransform)
{
    if (!wrapData.empty() && (wrap != wrapData.back().first || (align != wrapData.back().second && align != Font::ALIGN_MAX_ENUM)))
        throw love::Exception("addf cannot handle multiple wraps and aligns on this console.");

    this->textCache += this->GetString(text);

    this->wrap = wrap;
    this->align = align;

    return 0;
}

void Text::Draw(Graphics * gfx, const Matrix4 & localTransform)
{
    Matrix4 t(gfx->GetTransform(), localTransform);
    C2D_ViewRestore(&t.GetElements());

    Colorf color = gfx->GetColor();

    u32 renderColorf = C2D_Color32f(color.r, color.g, color.b, color.a);
    C2D_DrawText(&this->text, C2D_WithColor, 0, 0, Graphics::CURRENT_DEPTH, this->font->GetScale(), this->font->GetScale(), renderColorf);
}

void Text::Clear()
{
    C2D_TextBufClear(this->buffer);
    this->textCache.clear();
}