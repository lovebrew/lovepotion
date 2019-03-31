#pragma once

class Text : public Drawable
{
    public:
        Text() {};
        Text(Font * font, const char * text);

        void Clear();

        Font * GetFont();

        void SetFont(Font * font);
        void Set(const char * text, int limit, const std::string & align);

    private:
        Font * font;
};