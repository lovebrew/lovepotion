#pragma once

class Image : public Drawable
{
    public:
        Image() {};
        Image(const char * path, bool memory);
        ~Image();

    private:
        SDL_Surface * GetMemoryImage(const char * path);
};
