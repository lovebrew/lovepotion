#pragma once

class Font : public Drawable
{
    public:
        Font(const std::string & name, float size);
        Font(float size);
        Font();
        ~Font();

        float GetWidth(const char * text);
        float GetSize();

        void Print(const char * text, double x, double y, double rotation, double scalarX, double scalarY, Color color);
    private:
        C2D_Font LoadFont(const std::string & name);

	    C2D_TextBuf buffer;
        C2D_Text citroText;
        C2D_Font font;

        float size;
        float scale;
};
