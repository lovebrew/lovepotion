#pragma once

class SpriteBatch : public Drawable
{
    public:
        SpriteBatch(Image * image, int max);
        int Add(Viewport & view, float x, float y, float rotation, float scalarX, float scalarY);

    private:
        std::vector<Viewport> quads;

};