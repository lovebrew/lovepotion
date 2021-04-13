#include "objects/text/textc.h"

love::Type love::common::Text::type("Text", &Drawable::type);

using namespace love::common;

Text::Text(love::Font* font, const std::vector<Font::ColoredString>& text) : font(font)
{}

love::Font* Text::GetFont() const
{
    return this->font.Get();
}