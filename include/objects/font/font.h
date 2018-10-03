#pragma once

class Font : public Object
{
    public:
        Font(const char * path, int size);
        Font(int size = 16);
        ~Font();

        bool HasGlyph(uint glyph);
        int GetWidth(const char * text);
        int GetSize();
        TTF_Font * GetFont();
        void Print(const char * text, double x, double y, float limit, const std::string & align, SDL_Color color);

    private:
        TTF_Font * font;
        SDL_Texture * texture;
        float size;
};