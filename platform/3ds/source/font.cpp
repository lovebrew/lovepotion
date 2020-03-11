#include "common/runtime.h"
#include "objects/font/font.h"

using namespace love;

Font::Font(const std::string & path, float size) : buffer(C2D_TextBufNew(4096)),
                                                   size(size)
{
    this->font = this->LoadFromPath(path);
}

Font::Font(float size) : buffer(C2D_TextBufNew(4096)),
                         size(size)
{
    this->font = this->LoadFromPath("standard");
}

Font::~Font()
{
    C2D_TextBufDelete(this->buffer);
    C2D_FontFree(this->font);
}

FontHandle Font::LoadFromPath(const std::string & path)
{
    if (path == "standard")
        return C2D_FontLoadSystem(CFG_REGION_USA);
    else if (path == "korean")
        return C2D_FontLoadSystem(CFG_REGION_KOR);
    else if (path == "taiwanese")
        return C2D_FontLoadSystem(CFG_REGION_TWN);
    else if (path == "chinese")
        return C2D_FontLoadSystem(CFG_REGION_CHN);

    size_t pos = path.find_last_of(".") + 1;
    std::string translation;

    if (pos != std::string::npos)
        translation = (path.substr(0, pos) + "fnt");

    if (std::filesystem::exists(translation))
        return C2D_FontLoad(translation.c_str());

    return NULL;
}

void Font::Print(const std::vector<ColoredString> & strings, const DrawArgs & args, float * limit, const Color & color)
{
    std::pair<float, float> offset = {0.0f, 0.0f};
    std::string line = "";

    for (size_t i = 0; i < strings.size(); i++)
    {
        const std::string & str = strings[i].string;
        auto currentChar = str.begin();
        const Color & clr = strings[i].color;

        while (currentChar != str.end())
        {
            if (*currentChar == '\n')
            {
                std::pair<float, float> size = this->GenerateVertices(line, offset, args, clr);

                offset.first = 0.0f;
                offset.second += size.second;

                line = "";
            }
            else
                line += *currentChar;

            ++currentChar;
        }

        if (!line.empty())
        {
            std::pair<float, float> size = this->GenerateVertices(line, offset, args, clr);
            offset.first += size.first;

            line = "";
        }
    }
}

std::pair<float, float> Font::GenerateVertices(const std::string & line, const std::pair<float, float> & offset, const DrawArgs & args, const Color & color)
{
    C2D_Text text;
    const char * str = line.c_str();

    C2D_TextFontParse(&text, this->font, this->buffer, str);
    C2D_TextOptimize(&text);

    float width;
    float height;

    C2D_TextGetDimensions(&text, this->GetScale(), this->GetScale(), &width, &height);

    u32 currentColor = C2D_Color32f(color.r, color.g, color.b, color.a);
    C2D_DrawText(&text, C2D_WithColor, args.x + offset.first, args.y + offset.second, args.depth, this->GetScale() * args.scalarX, this->GetScale() * args.scalarY, currentColor);

    return std::pair(width, height);
}

void Font::ClearBuffer()
{
    C2D_TextBufClear(this->buffer);
}

float Font::GetWidth(const char * text)
{
    float width = 0;
    C2D_Text measureText;
    C2D_TextBuf measureBuffer = C2D_TextBufNew(strlen(text));

    C2D_TextFontParse(&measureText, this->font, measureBuffer, text);

    if (strlen(text) != 0)
        C2D_TextGetDimensions(&measureText, this->GetScale(), this->GetScale(), &width, NULL);

    C2D_TextBufDelete(measureBuffer);

    return width;
}

float Font::GetHeight()
{
    return 30 * this->GetScale();
}
