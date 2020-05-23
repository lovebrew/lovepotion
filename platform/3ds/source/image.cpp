#include "common/runtime.h"
#include "objects/image/image.h"

using namespace love;

#define IMAGE_NOT_FOUND_STRING "Could not find image %s (not converted to t3x?)"

Image::Image(Data * data) : Texture(Texture::TEXTURE_2D)
{
    this->sheet = C2D_SpriteSheetLoadFromMem(data->GetData(), data->GetSize());

    C2D_SpriteFromSheet(&this->texture, this->sheet, 0);

    this->width  = this->texture.image.subtex->width;
    this->height = this->texture.image.subtex->height;


    this->InitQuad();
}

Image::~Image()
{
    if (this->sheet != NULL)
        C2D_SpriteSheetFree(this->sheet);
}

void Image::Draw(const DrawArgs & args, const Color & color)
{
    this->Draw(args, this->quad, color);
}

void Image::Draw(const DrawArgs & args, love::Quad * quad, const Color & color)
{
    Quad::Viewport v = quad->GetViewport();
    Tex3DS_SubTexture source;

    /* Set up the Quad */

    int w =  quad->GetTextureWidth(true);
    int h =  quad->GetTextureHeight(true);

    v.x += 1;
    v.y += 1;

    v.w -= 2;
    v.h -= 2;

    source.top    = 1.0f - (v.y / h);
    source.left   = (v.x / w);

    source.right  = (v.x + v.w) / w;
    source.bottom = 1.0f - ((v.y + v.h) / h);

    source.width  = v.w;
    source.height = v.h;

    this->texture.image.subtex = &source;

    /*
    ** Note that the offsets are applied before rotation, scaling, or shearing;
    ** Scaling and shearing are applied before rotation.
    **
    ** Offset X and Y (aka centering values) always push the texture left and up
    ** as if they were negative
    */

   this->texture.params.pos.w = v.w;
   this->texture.params.pos.h = v.h;

    C2D_SpriteSetPos(&this->texture, args.x, args.y);
    C2D_SpriteSetDepth(&this->texture, args.depth);

    C2D_SpriteSetCenterRaw(&this->texture, args.offsetX, args.offsetY);
    C2D_SpriteSetScale(&this->texture, args.scalarX, args.scalarY);

    C2D_SpriteSetRotation(&this->texture, args.r);

    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, color.a);

    /* Render the texture */

    C2D_DrawSpriteTinted(&this->texture, &tint);
}
