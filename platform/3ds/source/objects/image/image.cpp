#include "common/runtime.h"
#include "common/drawable.h"
#include "objects/image/image.h"

#include "screen_png.h"
#include "bottom_png.h"
#include "eye_png.h"
#include "nogame_png.h"

#include "warn_png.h"
#include "plus_png.h"

Image::Image(const char * path, bool memory) : Drawable("Image")
{
    u32 * outBuffer = nullptr;

    if (!memory)
    {
        outBuffer = this->LoadPNG(path, NULL, -1);
        this->LoadImage(outBuffer);
    }
    else
    {
        int size;
        char * buffer = this->GetMemoryImage(path, &size);

        outBuffer = this->LoadPNG(NULL, buffer, size);
        this->LoadImage(outBuffer);
    }
}

char * Image::GetMemoryImage(const char * path, int * size)
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
    else if (name == "warn")
    {
        *size = warn_png_size;
        return (char *)warn_png;
    }
    else
    {
        *size = plus_png_size;
        return (char *)plus_png;
    }
}