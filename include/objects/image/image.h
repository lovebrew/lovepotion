#pragma once

class Image : public Drawable
{
    public:
        Image(const char * path, bool memory);
        ~Image();

        u32 * LoadPNG(const char * path, char * buffer, size_t memorySize);
        void LoadImage(u32 * buffer);

        char * GetMemoryImage(const char * path, int * size);

        C2D_Image GetImage();
};