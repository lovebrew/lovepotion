#include <objects/font_ext.hpp>

#include <objects/textbatch_ext.hpp>

using namespace love;

TextBatch<Console::HAC>::TextBatch(Font<Console::HAC>* font, const Font<>::ColoredStrings& text) :
    TextBatch<Console::ALL>(font)
{}

TextBatch<Console::HAC>::~TextBatch()
{}

void TextBatch<Console::HAC>::UploadVertices(const std::vector<vertex::Vertex>& vertices,
                                             size_t offset)
{}

void TextBatch<Console::HAC>::RegenerateVertices()
{}

void TextBatch<Console::HAC>::AddTextData(const TextBatch<Console::HAC>::TextData& data)
{}

void TextBatch<Console::HAC>::Clear()
{}

void TextBatch<Console::HAC>::AddText(const Font<>::ColoredStrings& text)
{}

void TextBatch<Console::HAC>::AddText(const Font<>::ColoredStrings& text,
                                      const Matrix4<Console::HAC>& matrix)
{}

void TextBatch<Console::HAC>::Set(const Font<>::ColoredStrings& text)
{
    this->Set(text, -1.0f, Font<>::ALIGN_MAX_ENUM);
}

void TextBatch<Console::HAC>::Set(const Font<>::ColoredStrings& text, float wrap,
                                  Font<>::AlignMode align)
{}

int TextBatch<Console::HAC>::Add(const Font<>::ColoredStrings& text,
                                 const Matrix4<Console::HAC>& transform)
{
    return this->Addf(text, -1.0f, Font<>::ALIGN_MAX_ENUM, transform);
}

int TextBatch<Console::HAC>::Addf(const Font<>::ColoredStrings& text, float wrap,
                                  Font<>::AlignMode mode, const Matrix4<Console::HAC>& transform)
{
    return 0;
}

void TextBatch<Console::HAC>::Draw(Graphics<Console::HAC>& graphics,
                                   const Matrix4<Console::Which>& localTransform)
{}

int TextBatch<Console::HAC>::GetWidth(int index) const
{
    return 0;
}

int TextBatch<Console::HAC>::GetHeight(int index) const
{
    return 0;
}
