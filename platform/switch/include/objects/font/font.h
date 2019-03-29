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
        void Print(const char * text, float x, float y, float rotation, float limit, float offsetX, float offsetY, float scalarX, float scalarY, SDL_Color color);

    private:
        TTF_Font * font;
        SDL_Texture * texture;
        float size;
};