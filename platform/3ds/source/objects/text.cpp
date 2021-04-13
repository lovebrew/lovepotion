#include "objects/text/text.h"
#include "modules/graphics/graphics.h"

#include <numeric>

using namespace love;

Text::Text(love::Font* font, const std::vector<Font::ColoredString>& text) :
    common::Text(font, text)
{
    this->buffer = C2D_TextBufNew(Font::FONT_BUFFER_SIZE);
    this->Set(text);
}

Text::~Text()
{
    C2D_TextBufDelete(this->buffer);
}

void Text::SetFont(love::Font* font)
{
    this->font.Set(font);
}

std::string Text::GetString(const std::vector<Font::ColoredString>& text)
{
    std::string result = std::accumulate(
        text.begin(), text.end(), std::string {},
        [](const std::string& s1, const Font::ColoredString& piece) { return s1 + piece.string; });

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
** a lot of .. text? Like using add(f)\
** So just return the entire string height
*/
int Text::GetHeight(int index) const
{
    return this->font->GetHeight();
}

void Text::Set(const std::vector<Font::ColoredString>& text)
{
    return this->Set(text, -1.0f, Font::ALIGN_MAX_ENUM);
}

void Text::Set(const std::vector<Font::ColoredString>& text, float wrap, Font::AlignMode align)
{
    if (text.empty() || (text.size() == 1 && text[0].string.empty()))
        return this->Clear();

    this->textCache = this->GetString(text);

    this->wrap  = wrap;
    this->align = align;

    C2D_TextBufClear(this->buffer);
    C2D_TextFontParse(&this->text, this->font->GetFont(), this->buffer, this->textCache.c_str());
    C2D_TextOptimize(&this->text);
}

int Text::Add(const std::vector<Font::ColoredString>& text, const Matrix4& localTransform)
{
    return this->Addf(text, -1.0f, Font::ALIGN_MAX_ENUM, localTransform);
}

/*
** Galaxy brain move here:
** always return 0 because we don't
** use a vector of data for strings on 3DS
*/
int Text::Addf(const std::vector<Font::ColoredString>& text, float wrap, Font::AlignMode align,
               const Matrix4& localTransform)
{
    if (!this->wrapData.empty() &&
        (wrap != this->wrapData.back().first ||
         (align != this->wrapData.back().second && align != Font::ALIGN_MAX_ENUM)))
        throw love::Exception("addf cannot handle multiple wraps and aligns on this console.");

    this->textCache += this->GetString(text);

    this->wrap  = wrap;
    this->align = align;

    this->wrapData.push_back(std::make_pair(wrap, align));

    C2D_TextBufClear(this->buffer);
    C2D_TextFontParse(&this->text, this->font->GetFont(), this->buffer, this->textCache.c_str());
    C2D_TextOptimize(&this->text);

    return 0;
}

void Text::Draw(Graphics* gfx, const Matrix4& localTransform)
{
    Matrix4 t(gfx->GetTransform(), localTransform);
    C2D_ViewRestore(&t.GetElements());

    Colorf color = gfx->GetColor();

    u32 renderColorf = C2D_Color32f(color.r, color.g, color.b, color.a);
    u32 flags        = C2D_WithColor;

    u32 alignMode = C2D_WordWrap;
    float offset  = 0.0f;

    switch (this->align)
    {
        case Font::ALIGN_LEFT:
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
        default:
            break;
    }

    if (this->align != Font::ALIGN_MAX_ENUM)
        flags |= alignMode;

    /* wrap will be discarded if there's no align mode specified */
    C2D_DrawText(&this->text, flags, offset, 0, Graphics::CURRENT_DEPTH, this->font->GetScale(),
                 this->font->GetScale(), renderColorf, this->wrap);
}

void Text::Clear()
{
    C2D_TextBufClear(this->buffer);
    this->textCache.clear();
    this->wrapData.clear();
}