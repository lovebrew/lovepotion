#pragma once

class ImageData : public Object
{
    public:
        ImageData(int width, int height);
        ImageData(int width, int height, const std::string & format);
        ImageData(const char * filename);

        SDL_Color GetPixel(int x, int y);

        u32 * GetBuffer();
        int GetSize();

    private:
        u32 * LoadFilename(const char * filename);
        u32 * LoadBuffer();

        int width;
        int height;

        u32 * buffer;
        int size;
};