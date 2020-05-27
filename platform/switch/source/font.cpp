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

Font::Font(Font::SystemFontType type, float size) : size(size),
                                                   texture(nullptr)
{
    PlFontData data;
    plGetSharedFontByType(&data, (PlSharedFontType)type);

    this->font = TTF_OpenFontRW(SDL_RWFromMem(data.address, data.size), 1, size);
}

Font::Font(float size) : size(size),
                         texture(nullptr)
{
    PlFontData data;
    plGetSharedFontByType(&data, (PlSharedFontType)SystemFontType::TYPE_STANDARD);

    this->font = TTF_OpenFontRW(SDL_RWFromMem(data.address, data.size), 1, size);
}

Font::~Font()
{
    TTF_CloseFont(this->font);
}

std::pair<float, float> Font::GenerateVertices(const std::string & line, const std::pair<float, float> & offset,
                                               const DrawArgs & args, const Color & blend, const Color & color)
{
    const char * str = line.c_str();

    int width;
    int height;

    SDL_Surface * temp = TTF_RenderUTF8_Blended(this->font, str, {(uint8_t)blend.r, (uint8_t)blend.g, (uint8_t)blend.b, (uint8_t)blend.a});
    SDL_Texture * lTexture = SDL_CreateTextureFromSurface(WINDOW_MODULE()->GetRenderer(), temp);

    SDL_FreeSurface(temp);

    SDL_QueryTexture(lTexture, NULL, NULL, &width, &height);

    SDL_RendererFlip flipHorizonal = (args.scalarX < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RendererFlip flipVertical  = (args.scalarY < 0) ? SDL_FLIP_VERTICAL   : SDL_FLIP_NONE;

    SDL_Rect source({0, 0, width, height});
    SDL_Rect destin({(int)round(args.x + offset.first),
                    (int)round(args.y + offset.second),
                    (int)ceil(width * args.scalarX),
                    (int)ceil(height * args.scalarY)});

    SDL_SetTextureBlendMode(lTexture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureColorMod(lTexture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(lTexture, color.a);

    SDL_Point center = {(int)(args.offsetX * args.scalarX), (int)(args.offsetY * args.scalarY)};
    double rotation = (args.r * 180 / M_PI);

    SDL_RenderCopyEx(WINDOW_MODULE()->GetRenderer(), lTexture, &source, &destin, rotation, &center, (SDL_RendererFlip)(flipVertical | flipHorizonal));

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

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM>::Entry Font::sharedFontEntries[] =
{
    { "standard",                    TYPE_STANDARD               },
    { "chinese simplified",          TYPE_CHINESE_SIMPLIFIED     },
    { "chinese traditional",         TYPE_CHINESE_TRADITIONAL    },
    { "extended chinese simplified", TYPE_CHINESE_SIMPLIFIED_EXT },
    { "korean",                      TYPE_KOREAN                 },
    { "nintendo extended",           TYPE_NINTENDO_EXTENDED      }
};

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM> Font::sharedFonts(Font::sharedFontEntries, sizeof(Font::sharedFontEntries));
