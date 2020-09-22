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

void Font::RenderLine(const std::string & line, love::Vector2 & offset, const DrawArgs & args,
                      const Color & blend, float wrap, Font::AlignMode align, bool isNewLine)
{
    const char * str = line.c_str();
    int width = 0;
    int height = 0;
    float extraSpacing = 0.0f;

    SDL_Surface * tempSurf = TTF_RenderUTF8_Blended(this->font, str, {(uint8_t)blend.r, (uint8_t)blend.g, (uint8_t)blend.b, (uint8_t)blend.a});
    SDL_Texture * lTexture = SDL_CreateTextureFromSurface(WINDOW_MODULE()->GetRenderer(), tempSurf);

    SDL_FreeSurface(tempSurf);

    SDL_QueryTexture(lTexture, NULL, NULL, &width, &height);

    switch (align)
    {
        case ALIGN_RIGHT:
            offset.x = floorf(wrap - width);
            break;
        case ALIGN_CENTER:
            offset.x = floorf((wrap - width) / 2.0f);
            break;
        case ALIGN_JUSTIFY:
        {
            float numspaces = (float)std::count(line.begin(), line.end(), ' ');
            if (width < wrap && numspaces >= 1)
                extraSpacing = (wrap - width) / numspaces;
            else
                extraSpacing = 0.0f;
            break;
        }
        case ALIGN_LEFT:
        default:
            break;
    }

    SDL_RendererFlip flipHorizonal = (args.scalarX < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RendererFlip flipVertical  = (args.scalarY < 0) ? SDL_FLIP_VERTICAL   : SDL_FLIP_NONE;

    SDL_Rect source({0, 0, width, height});
    SDL_Rect destin({(int)round(args.x + offset.x),
                    (int)round(args.y  + offset.y),
                    (int)ceil(width * args.scalarX),
                    (int)ceil(height * args.scalarY)});

    SDL_SetTextureBlendMode(lTexture, SDL_BLENDMODE_BLEND);

    // SDL_SetTextureColorMod(lTexture, color.r, color.g, color.b);
    // SDL_SetTextureAlphaMod(lTexture, color.a);

    SDL_Point center = {(int)(args.offsetX * args.scalarX), (int)(args.offsetY * args.scalarY)};
    double rotation = (args.r * 180 / M_PI);

    SDL_RenderCopyEx(WINDOW_MODULE()->GetRenderer(), lTexture, &source, &destin, rotation, &center, (SDL_RendererFlip)(flipVertical | flipHorizonal));

    SDL_DestroyTexture(lTexture);

    if (isNewLine)
    {
        offset.x = 0.0;
        offset.y += height;
    }
    else
        offset.x += width + extraSpacing;
}

void Font::Print(const std::vector<Font::ColoredString> & strings, const DrawArgs & args,
                 float limit, const Color & blend, Font::AlignMode align)
{
    love::Vector2 offset(0, 0);
    float size = 0.0f;
    std::string line;

    for (size_t i = 0; i < strings.size(); i++)
    {
        const std::string & str = strings[i].string;
        auto currentChar = str.c_str();
        const auto end = currentChar + str.size();
        const Color & clr = strings[i].color;

        while (currentChar != end)
        {
            uint32_t codepoint;
            const auto bytes = decode_utf8(&codepoint, (uint8_t *)currentChar);

            if (codepoint == '\n' || (limit > 0 && size >= limit))
                this->RenderLine(line, offset, args, Colors::ALPHA_BLEND_COLOR(blend, clr), limit, align, true);

            if (codepoint == '\n')
                line.clear();
            else
            {
                if (limit > 0 && size >= limit)
                {
                    size = 0.0f;
                    line.clear();
                }

                line.append(currentChar, bytes);
            }

            currentChar += bytes;
        }

        if (!line.empty())
        {
            this->RenderLine(line, offset, args, Colors::ALPHA_BLEND_COLOR(blend, clr), limit, align);
            line.clear();
        }
    }
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
