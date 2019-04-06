#include "common/runtime.h"
#include "modules/graphics.h"
#include "common/drawable.h"
#include "objects/image/image.h"

#include "screen_png.h"
#include "bottom_png.h"
#include "eye_png.h"
#include "nogame_png.h"

#include "warn_sm_png.h"
#include "button_sm_png.h"

Image::Image(const char * path, bool memory) : Drawable("Image")
{
    C2D_SpriteSheet sheet = NULL;

    if (!memory)
    {
        sheet = C2D_SpriteSheetLoad(path);
        this->image = C2D_SpriteSheetGetImage(sheet, 0);
    }
    else
    {
        size_t size;
        char * buffer = this->GetMemoryImage(path, &size);

        sheet = C2D_SpriteSheetLoadFromMem(buffer, size);
        this->image = C2D_SpriteSheetGetImage(sheet, 0);
    }

    C3D_TexSetFilter(this->image.tex, magFilter, minFilter);
    
    this->image.tex->border = 0x00FFFFFF;
    
    C3D_TexSetWrap(this->image.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

char * Image::GetMemoryImage(const char * path, size_t * size)
{
    string name = path;
    name = name.substr(name.find(":") + 1);

    if (name == "screen")
    {
        *size = screen_png_size;
        return (char *)screen_png;
    }
    else if (name == "bottom")
    {
        *size = bottom_png_size;
        return (char *)bottom_png;
    }
    else if (name == "eye")
    {
        *size = eye_png_size;
        return (char *)eye_png;
    }
    else if (name == "nogame")
    {
        *size = nogame_png_size;
        return (char *)nogame_png;
    }
    else if (name == "warn_sm")
    {
        *size = warn_sm_png_size;
        return (char *)warn_sm_png;
    }
    
    *size = button_sm_png_size;
    return (char *)button_sm_png;
}