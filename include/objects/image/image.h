#pragma once

class Image : public Drawable
{
    public:
        Image(const char * path, bool memory);
        ~Image();

        u32 * LoadPNG(const char * path, size_t memorySize);
        void LoadImage(u32 * buffer);

        C2D_Image GetImage();
};