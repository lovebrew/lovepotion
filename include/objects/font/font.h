#pragma once

class Font : public Drawable
{
    public:
        Font(const char * path, int size);
        Font(int size = 16);
        ~Font();

        bool HasGlyph(uint glyph);
        int GetWidth(const char * text);
        int GetSize();
        TTF_Font * GetFont();
        void Print(const char * text, double x, double y, double rotation, float limit, double scalarX, double scalarY, SDL_Color color);

    private:
        TTF_Font * font;
        SDL_Texture * texture;
        float size;
};