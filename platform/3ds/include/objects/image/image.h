#pragma once

class Image : public Drawable
{
    public:
        Image(const char * path, bool memory);
        ~Image();

        char * GetMemoryImage(const char * path, int * size);

        C2D_Image GetImage();
};