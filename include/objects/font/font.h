#pragma once

class Font : public Drawable
{
    public:
        Font(const string & path);
        Font();

        bool HasGlyph(uint glyph);
        int GetWidth(const char * text);
        int GetSize();
        std::map<int, Glyph> GetGlyphs();

        void Print(const char * text, double x, double y, double rotation, double scalarX, double scalarY, C2D_ImageTint color);
    private:
        void LoadAssets(const string & path);

        void LoadJSON(json_t * json);
        void LoadGlyph(json_t * object);
        void LoadKerning(json_t * object);
        int LoadGlyphInfo(json_t * object, const char * key);

        std::map<int, Glyph> glyphs;
        std::map<int, std::map<int, int>> kernings; //kernings[code][next] -> kerning value returned

        int size;
};