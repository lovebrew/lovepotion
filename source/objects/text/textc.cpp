#include "common/runtime.h"
#include "objects/text/textc.h"

love::Type love::common::Text::type("Text", &Drawable::type);

using namespace love::common;

Text::Text(Font * font, const std::vector<Font::ColoredString> & text) : font(font)
{
    this->Set(text);
}

Font * Text::GetFont() const
{
    return this->font.Get();
}