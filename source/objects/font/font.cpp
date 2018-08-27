#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/quad/quad.h"
#include "objects/font/font.h"

#include "vera_14_png.h"
#include "vera_14_json.h"

Font::Font(const string & path) : Drawable("Font")
{
    string basePath = path.substr(0, path.find_last_of("."));

    this->LoadAssets(basePath);
}

Font::Font() : Drawable("Font")
{
    this->LoadAssets("");
}

void Font::LoadAssets(const string & path)
{
    json_t * root = nullptr;
    json_error_t error;
    u32 * buffer = nullptr;

    if (path != "")
    {
        string pngPath = path + ".png";
        string jsonPath = path + ".json";

        buffer = this->LoadPNG(pngPath.c_str(), NULL, -1);
        root = json_load_file(jsonPath.c_str(), 0, &error);
    }
    else
    {
        buffer = this->LoadPNG(NULL, (char *)vera_14_png, vera_14_png_size);
        root = json_loadb((char *)vera_14_json, vera_14_json_size, 0, &error);
    }

    if (buffer)
        this->LoadImage(buffer);

    if (json_is_object(root))
        this->LoadJSON(root);

}

void Font::LoadJSON(json_t * json)
{
    /* array is a JSON array */
    size_t charsIndex;
    json_t * charsValue;
    
    json_t * chars = json_object_get(json, "chars");
    json_array_foreach(chars, charsIndex, charsValue)
        this->LoadGlyph(charsValue);

    json_t * common = json_object_get(json, "common");
    this->size = this->LoadGlyphInfo(common, "lineHeight");

    size_t kerningIndex;
    json_t * kerningValue;

    /*json_t * kerningTree = json_object_get(json, "kernings");
    json_array_foreach(kerningTree, kerningIndex, kerningValue)
        this->LoadKerning(kerningValue);*/
}

void Font::LoadKerning(json_t * object)
{
    if (!json_is_object(object))
        return;

    int first = this->LoadGlyphInfo(object, "first");
    int second = this->LoadGlyphInfo(object, "second");
    
    int amount = this->LoadGlyphInfo(object, "amount");

    this->kernings[first][second] = amount; //std::map<int, std::map<int, int>>
}

void Font::LoadGlyph(json_t * object)
{
    if (!json_is_object(object))
        return;

    Glyph glyph;
    u32 w_pow2 = NextPO2((u32)this->width);
    u32 h_pow2 = NextPO2((u32)this->height);

    int id = this->LoadGlyphInfo(object, "id");

    int x = this->LoadGlyphInfo(object, "x");
    int y = this->LoadGlyphInfo(object, "y");

    int width = this->LoadGlyphInfo(object, "width");
    int height = this->LoadGlyphInfo(object, "height");

    int xoffset = this->LoadGlyphInfo(object, "xoffset");
    int yoffset = this->LoadGlyphInfo(object, "yoffset");
    int xadvance = this->LoadGlyphInfo(object, "xadvance");

    Quad quad(x, y, width, height, w_pow2, h_pow2);

    Tex3DS_SubTexture subTexture;
    quad.SetSubTexture(&subTexture);

    glyph.id = id;
    glyph.xoffset = xoffset;
    glyph.yoffset = yoffset;

    //glyph.width = width - xoffset;

    glyph.xadvance = xadvance;
    glyph.subTexture = subTexture;

    this->glyphs[id] = glyph;
}

int Font::LoadGlyphInfo(json_t * object, const char * key)
{
    json_t * value = json_object_get(object, key);
    return (int)json_integer_value(value);
}

void Font::Print(const char * text, double x, double y, double rotation, double scalarX, double scalarY, Color color)
{
    int last = 0;
    int kerning = 0;
    int startx = x;
    const uint8_t * utf8 = (const uint8_t *)text;

    while (*utf8 != 0)
    {
        uint32_t code;

        ssize_t units = decode_utf8(&code, utf8);

        if (units == -1)
        {
            code = 0xFFFD;
            units = 1;
        }
        else if (code == '\n')
        {
            x = startx;
            y = y + this->GetSize();
            last = -1;
        }
        utf8 += units;
        
        if (code != '\n' && this->glyphs.find(code) == this->glyphs.end())
            continue;

        if (code > 0 && last != -1)
            x += (this->glyphs[last].xadvance);

        this->SetSubTexture(this->glyphs[code].subTexture);

        this->Draw(x + this->glyphs[code].xoffset + kerning, y + this->glyphs[code].yoffset, 0, 1, 1, color);

        last = code;
    }
}

int Font::GetWidth(const char * text)
{
    int width = 0;

    if (strlen(text) != 0)
        printf("not yet implemented\n");

    return width;
}

int Font::GetSize()
{
    return this->size;
}

bool Font::HasGlyph(uint glyph)
{
    return false;
}