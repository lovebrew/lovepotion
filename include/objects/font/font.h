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

        void Print(const char * text, size_t length, double x, double y, Color color);
    private:
        void LoadAssets(const string & path);

        void LoadJSON(json_t * json);
        void LoadGlyph(json_t * object);
        void LoadKerning(json_t * json);
        int LoadGlyphInfo(json_t * object, const char * key);

        std::map<int, Glyph> glyphs;

        int size;
};