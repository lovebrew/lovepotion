#include "common/runtime.h"

#include "modules/window/window.h"
#include "objects/font/font.h"

using namespace love;

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
        return TTF_OpenFont(path.c_str(), this->size);
    else
    {
        plGetSharedFontByType(&data, type);
        return TTF_OpenFontRW(SDL_RWFromMem(data.address, data.size), 1, this->size);
    }
}

void Font::Print(const char * string, const DrawArgs & args, float * limit, const Color & color)
{
    if (strlen(string) == 0)
        return;

    SDL_Surface * temp = nullptr;

    if (limit != nullptr)
        temp = TTF_RenderUTF8_Blended_Wrapped(this->font, string, {color.r, color.g, color.b, color.a}, *limit);
    else
        temp = TTF_RenderUTF8_Blended(this->font, string, {color.r, color.g, color.b, color.a});

    auto window = Module::GetInstance<Window>(Module::M_WINDOW);
    this->texture = SDL_CreateTextureFromSurface(window->GetRenderer(), temp);

    SDL_FreeSurface(temp);

    int width, height = 0;
    SDL_QueryTexture(this->texture, nullptr, nullptr, &width, &height);

    SDL_Rect source({0, 0, width, height});
    SDL_Rect destin({(int)round(args.x), (int)round(args.y), (int)ceil(width), (int)ceil(height)});

    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);

    SDL_RenderCopyEx(window->GetRenderer(), this->texture, &source, &destin, args.r, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);

    SDL_DestroyTexture(this->texture);
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

float Font::GetWidth(const char * text)
{
    int width = 0;

    if (strlen(text) != 0)


    return width;
}
