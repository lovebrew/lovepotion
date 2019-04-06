#include "common/runtime.h"

#include "modules/filesystem.h"

#include "common/drawable.h"
#include "objects/font/font.h"
#include "modules/graphics.h"
#include "modules/window.h"

Font::Font(const char * path, int size) : Drawable("Font")
{
    float dpiScale = 1.0f;
    this->size = floorf(size * dpiScale + 0.5f);

    this->font = TTF_OpenFont(path, this->size);
    this->texture = NULL;

    if (!this->font)
        printf("Font error: %s", TTF_GetError());
}

Font::Font(int size) : Drawable("Font")
{
    vector<PlSharedFontType> type = {PlSharedFontType_Standard, PlSharedFontType_NintendoExt};
    
    uint8_t * concatFontData = (uint8_t *)malloc(1);
    uint8_t * tempConcatData = (uint8_t *)malloc(1);

    u32 totalSize = 0;
    u32 progress = 0;

    for (uint i = 0; i < type.size(); i++)
    {
        PlFontData font;
        plGetSharedFontByType(&font, type[i]);

        u32 plFontSize = font.size;
        totalSize += plFontSize;

        tempConcatData = (uint8_t *)realloc(concatFontData, totalSize);

        if (tempConcatData != NULL)
        {
            memcpy(&tempConcatData[progress], font.address, plFontSize);
    
            progress += plFontSize;
            
            concatFontData = tempConcatData;
        }
        else
            free(concatFontData);
    }
    
    float dpiScale = 1.0f;
    size = floorf(size * dpiScale + 0.5f);

    this->font = TTF_OpenFontRW(SDL_RWFromMem((void *)concatFontData, totalSize), 1, size);

    this->texture = NULL;
}

void Font::Print(const char * text, float x, float y, float rotation, float limit, float offsetX, float offsetY, float scalarX, float scalarY, SDL_Color color)
{
    if (strlen(text) == 0)
        return;

    SDL_Surface * tempSurface = TTF_RenderUTF8_Blended_Wrapped(this->font, text, color, limit);
    
    this->texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), tempSurface);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    SDL_FreeSurface(tempSurface);

    int width, height = 0;
    SDL_QueryTexture(this->texture, NULL, NULL, &width, &height);

    Viewport viewport = {0, 0, width, height, width, height};

    Drawable::Draw(texture, viewport, x, y, rotation, offsetX, offsetY, scalarX, scalarY, color);

    SDL_DestroyTexture(texture);
}

Font::~Font()
{
    TTF_CloseFont(this->font);
}

int Font::GetSize()
{
    return TTF_FontHeight(this->font);
}

int Font::GetWidth(const char * text)
{
    int width = 0;

    if (strlen(text) != 0)
        TTF_SizeUTF8(this->font, text, &width, NULL);

    return width;
}

TTF_Font * Font::GetFont()
{
    return this->font;
}

bool Font::HasGlyph(uint glyph)
{
    return TTF_GlyphIsProvided(this->font, glyph);
}