#include "common/runtime.h"
#include "modules/graphics.h"
#include "common/drawable.h"
#include "objects/image/image.h"

#include "screen_t3x.h"
#include "bottom_t3x.h"
#include "eye_t3x.h"
#include "nogame_t3x.h"

#include "warn_sm_t3x.h"
#include "button_sm_t3x.h"

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
        const void * buffer = this->GetMemoryImage(path, &size);

        sheet = C2D_SpriteSheetLoadFromMem(buffer, size);
        this->image = C2D_SpriteSheetGetImage(sheet, 0);
    }

    C3D_TexSetFilter(this->image.tex, magFilter, minFilter);
    
    this->image.tex->border = 0x00FFFFFF;
    
    C3D_TexSetWrap(this->image.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

const void * Image::GetMemoryImage(const char * path, size_t * size)
{
    string name = path;
    name = name.substr(name.find(":") + 1);

    if (name == "screen")
    {
        *size = screen_t3x_size;
        return screen_t3x;
    }
    else if (name == "bottom")
    {
        *size = bottom_t3x_size;
        return bottom_t3x;
    }
    else if (name == "eye")
    {
        *size = eye_t3x_size;
        return eye_t3x;
    }
    else if (name == "nogame")
    {
        *size = nogame_t3x_size;
        return nogame_t3x;
    }
    else if (name == "warn_sm")
    {
        *size = warn_sm_t3x_size;
        return warn_sm_t3x;
    }
    
    *size = button_sm_t3x_size;
    return button_sm_t3x;
}