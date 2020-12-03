#include "common/runtime.h"
#include "objects/text/text.h"

using namespace love;

Text::Text(Font * font, const std::vector<Font::ColoredString> & text) : common::Text(font, text)
{}

void Text::Set(const std::vector<Font::ColoredString> & text)
{
    return this->Set(text, -1.0f, Font::ALIGN_MAX_ENUM);
}

void Text::Set(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align)
{
    if (text.empty() || text.size() == 1 && text[0].string.empty())
        return this->Clear();
}