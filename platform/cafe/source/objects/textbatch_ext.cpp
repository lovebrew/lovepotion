#include <objects/font_ext.hpp>

#include <objects/textbatch_ext.hpp>

using namespace love;

TextBatch<Console::CAFE>::TextBatch(Font<Console::CAFE>* font, const Font<>::ColoredStrings& text) :
    TextBatch<Console::ALL>(font)
{}

TextBatch<Console::CAFE>::~TextBatch()
{}

void TextBatch<Console::CAFE>::UploadVertices(const std::vector<vertex::Vertex>& vertices,
                                              size_t offset)
{}

void TextBatch<Console::CAFE>::RegenerateVertices()
{}

void TextBatch<Console::CAFE>::AddTextData(const TextBatch<Console::CAFE>::TextData& data)
{}

void TextBatch<Console::CAFE>::Clear()
{}

void TextBatch<Console::CAFE>::AddText(const Font<>::ColoredStrings& text)
{}

void TextBatch<Console::CAFE>::AddText(const Font<>::ColoredStrings& text,
                                       const Matrix4<Console::CAFE>& matrix)
{}

void TextBatch<Console::CAFE>::Set(const Font<>::ColoredStrings& text)
{
    this->Set(text, -1.0f, Font<>::ALIGN_MAX_ENUM);
}

void TextBatch<Console::CAFE>::Set(const Font<>::ColoredStrings& text, float wrap,
                                   Font<>::AlignMode align)
{}

int TextBatch<Console::CAFE>::Add(const Font<>::ColoredStrings& text,
                                  const Matrix4<Console::CAFE>& transform)
{
    return this->Addf(text, -1.0f, Font<>::ALIGN_MAX_ENUM, transform);
}

int TextBatch<Console::CAFE>::Addf(const Font<>::ColoredStrings& text, float wrap,
                                   Font<>::AlignMode mode, const Matrix4<Console::CAFE>& transform)
{
    return 0;
}

void TextBatch<Console::CAFE>::Draw(Graphics<Console::CAFE>& graphics,
                                    const Matrix4<Console::Which>& localTransform)
{}

int TextBatch<Console::CAFE>::GetWidth(int index) const
{
    return 0;
}

int TextBatch<Console::CAFE>::GetHeight(int index) const
{
    return 0;
}
