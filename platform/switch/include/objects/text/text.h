#pragma once

class Text : public Drawable
{
    public:
        Text() {};
        Text(Font * font, const char * text);

    private:
        Font * font;
};