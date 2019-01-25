#pragma once

class Font : public Drawable
{
    public:
        Font(float size);
        Font();
        ~Font();

        float GetWidth(const char * text);
        float GetSize();

        void Print(const char * text, double x, double y, double rotation, double scalarX, double scalarY, Color color);
    private:
        C2D_TextBuf buffer;
        C2D_Text citroText;

        float size;
        float scale;
};