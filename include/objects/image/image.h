#pragma once

class Image : public Drawable
{
    public:
        Image() {};
        Image(const char * path, bool memory);
        Image(const char * buffer, size_t bufferSize);
        ~Image();

    private:
        SDL_Surface * GetMemoryImage(const char * path);
};
