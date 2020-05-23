#include "common/runtime.h"

#include "modules/window/window.h"
#include "objects/font/font.h"

using namespace love;

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

Font::Font(Data * data, float size) : size(size),
                                      texture(nullptr)
{
    this->font = TTF_OpenFontRW(SDL_RWFromMem(data->GetData(), data->GetSize()), 1, size);
}

Font::Font(const std::string & path, float size) : size(size),
                                                   texture(nullptr)
{
    this->font = this->LoadFromPath(path);
}

Font::Font(float size) : size(size),
                         texture(nullptr)
{
    this->font = this->LoadFromPath("standard");
}

Font::~Font()
{
    TTF_CloseFont(this->font);
}

FontHandle Font::LoadFromPath(const std::string & path)
{
    PlFontData data;
    PlSharedFontType type = PlSharedFontType_Total;

    if (path == "chinese simplified")
        type = PlSharedFontType_ChineseSimplified;
    else if (path == "extended chinese simplified")
        type = PlSharedFontType_ExtChineseSimplified;
    else if (path == "chinese traditional")
        type = PlSharedFontType_ChineseTraditional;
    else if (path == "korean")
        type = PlSharedFontType_KO;
    else if (path == "nintendo extended")
        type = PlSharedFontType_NintendoExt;
    else if (path == "standard")
        type = PlSharedFontType_Standard;

    if (type == PlSharedFontType_Total)
        throw love::Exception("Invalid font type %s", path.c_str());

    plGetSharedFontByType(&data, type);

    SDL_RWops * ops = SDL_RWFromMem(data.address, data.size);
    return TTF_OpenFontRW(ops, 1, this->size);
}

std::pair<float, float> Font::GenerateVertices(const std::string & line, const std::pair<float, float> & offset,
                                               const DrawArgs & args, const Color & blend, const Color & color)
{
    const char * str = line.c_str();

    int width;
    int height;

    SDL_Surface * temp = TTF_RenderUTF8_Blended(this->font, str, {blend.r, blend.g, blend.b, blend.a});
    SDL_Texture * lTexture = SDL_CreateTextureFromSurface(WINDOW_MODULE()->GetRenderer(), temp);

    SDL_FreeSurface(temp);

    SDL_QueryTexture(lTexture, NULL, NULL, &width, &height);

    SDL_Rect source({0, 0, width, height});
    SDL_Rect destin({(int)round(args.x + offset.first),
                    (int)round(args.y + offset.second),
                    (int)ceil(width),
                    (int)ceil(height)});

    SDL_SetTextureBlendMode(lTexture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureColorMod(lTexture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(lTexture, color.a);

    SDL_RenderCopyEx(WINDOW_MODULE()->GetRenderer(), lTexture, &source, &destin, args.r, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);

    SDL_DestroyTexture(lTexture);

    return std::pair(width, height);
}

float Font::GetSize()
{
    return TTF_FontHeight(this->font);
}

float Font::GetHeight()
{
    int height = 0;
    TTF_SizeUTF8(this->font, " ", nullptr, &height);

    return height;
}

float Font::_GetGlyphWidth(u16 glyph)
{
    int xAdvance = 0;

    TTF_GlyphMetrics(this->font, glyph, nullptr, nullptr, nullptr, nullptr, &xAdvance);

    return xAdvance;
}

float Font::GetWidth(const char * text)
{
    int width = 0;

    if (strlen(text) != 0)
        TTF_SizeUTF8(this->font, text, &width, nullptr);

    return width;
}

void Font::ClearBuffer()
{}
