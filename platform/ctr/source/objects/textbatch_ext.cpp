#include <objects/textbatch_ext.hpp>

#include <objects/font_ext.hpp>

using namespace love;

TextBatch<Console::CTR>::TextBatch(Font<Console::CTR>* font, const Font<>::ColoredStrings& text) :
    TextBatch<Console::ALL>(font)
{
    this->buffer = C2D_TextBufNew(Font<Console::CTR>::TEXT_BUFFER_SIZE);
    this->Set(text);

    this->scale = font->GetScale();
}

TextBatch<Console::CTR>::~TextBatch()
{
    C2D_TextBufDelete(this->buffer);
}

void TextBatch<Console::CTR>::Clear()
{
    C2D_TextBufClear(this->buffer);
    this->wrapping.clear();
}

static std::string stringify(const std::string& string, const std::string other)
{
    return string + other;
}

void TextBatch<Console::CTR>::AddText(const Font<>::ColoredStrings& text)
{
    for (size_t index = 0; index < text.size(); index++)
    {
        this->cache.push_back(text[index].string);

        const int width  = this->font->GetWidth(text[index].string);
        const int height = this->font->GetHeight();

        this->textInfo.push_back({ .width = width, .height = height });
    }

    this->vertices =
        std::accumulate(this->cache.begin(), this->cache.end(), std::string {}, stringify);
}

/* TODO */
void TextBatch<Console::CTR>::AddText(const Font<>::ColoredStrings& text,
                                      const Matrix4<Console::CTR>& matrix)
{}

void TextBatch<Console::CTR>::Set(const Font<>::ColoredStrings& text)
{
    this->Set(text, -1.0f, Font<>::ALIGN_MAX_ENUM);
}

void TextBatch<Console::CTR>::Set(const Font<>::ColoredStrings& text, float wrap,
                                  Font<>::AlignMode align)
{
    // if (text.empty() || (text.size() == 1 && text[0].string.empty()))
    //     return this->Clear();

    // this->AddText(text);

    // this->wrap  = wrap;
    // this->align = align;

    // C2D_TextBufClear(this->buffer);
    // C2D_TextFontParse(&this->text, this->font->GetFont(), this->buffer, this->vertices.c_str());
    // C2D_TextOptimize(&this->text);
}

int TextBatch<Console::CTR>::Add(const Font<>::ColoredStrings& text,
                                 const Matrix4<Console::CTR>& transform)
{
    return this->Addf(text, -1.0f, Font<>::ALIGN_MAX_ENUM, transform);
}

int TextBatch<Console::CTR>::Addf(const Font<>::ColoredStrings& text, float wrap,
                                  Font<>::AlignMode mode, const Matrix4<Console::CTR>& transform)
{
    // if (!this->wrapping.empty() &&
    //     (wrap != this->wrapping.back().limit ||
    //      (align != this->wrapping.back().mode && align != Font<>::ALIGN_MAX_ENUM)))
    // {
    //     throw love::Exception("addf cannot handle multiple wraps and aligns on this console.");
    // }

    // this->AddText(text);

    // this->wrap  = wrap;
    // this->align = align;

    // C2D_TextBufClear(this->buffer);
    // C2D_TextFontParse(&this->text, this->font->GetFont(), this->buffer, this->vertices.c_str());
    // C2D_TextOptimize(&this->text);

    // return (int)this->cache.size() - 1;
    return 0;
}

void TextBatch<Console::CTR>::Draw(Graphics<Console::CTR>& graphics,
                                   const Matrix4<Console::Which>& localTransform)
{
    // Matrix4<Console::CTR> transform(graphics.GetTransform(), localTransform);
    // C2D_ViewRestore(&transform.GetElements());

    // uint32_t flags     = C2D_WithColor;
    // uint32_t alignMode = C2D_WordWrap;
    // float offset       = 0.0f;

    // if (this->align != Font<>::ALIGN_MAX_ENUM)
    // {
    //     std::optional<uint32_t> mode;
    //     if (!(mode = Font<Console::CTR>::textWrapModes.Find(align)))
    //         mode = C2D_AlignLeft;

    //     alignMode |= *mode;

    //     if (this->align == Font<>::ALIGN_CENTER)
    //         offset += wrap / 2;

    //     flags |= alignMode;
    // }

    // const auto color = graphics.GetColor().rgba();
    // C2D_DrawText(&text, flags, offset, 0, graphics.GetCurrentDepth(), this->scale, this->scale,
    //              color, wrap);
}

int TextBatch<Console::CTR>::GetWidth(int index) const
{
    if (index < 0)
        index = std::max((int)this->cache.size() - 1, 0);

    if (index >= (int)this->cache.size())
        return 0;

    return this->textInfo[index].width;
}

int TextBatch<Console::CTR>::GetHeight(int index) const
{
    if (index < 0)
        index = std::max((int)this->cache.size() - 1, 0);

    if (index >= (int)this->cache.size())
        return 0;

    return this->textInfo[index].height;
}
