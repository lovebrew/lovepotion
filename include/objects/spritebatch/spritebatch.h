#pragma once

/*
<fincs> You need to define a SpriteBatch object and record information inside
<TurtleP> yeah
<fincs> And when it's time to draw it, just call C2D_Draw* for each sprite
*/

class SpriteBatch
{
    public:
        SpriteBatch(C2D_Image image, int maxSprites);
        int Add(float x, float y, float rotation, float scalarX, float scalarY, float offsetX, float offsetY);
        int Add(Tex3DS_SubTexture subtexture, float x, float y, float rotation, float scalarX, float scalarY, float offsetX, float offsetY);

    private:
        std::vector<std::pair<C2D_Image, Tex3DS_SubTexture>> images;
};