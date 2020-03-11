#include "common/runtime.h"
#include "objects/image/image.h"

using namespace love;

Image::Image(const std::string & path) : Texture(Texture::TEXTURE_2D)
{
    size_t pos = path.find_last_of(".") + 1;
    std::string translation;

    if (pos != std::string::npos)
        translation = (path.substr(0, pos) + "t3x");

    if (std::filesystem::exists(translation))
    {
        C2D_SpriteSheet sheet = C2D_SpriteSheetLoad(translation.c_str());
        this->texture = C2D_SpriteSheetGetImage(sheet, 0);

        if (!this->texture.subtex)
        {
            this->width = this->texture.tex->width;
            this->height = this->texture.tex->height;
        }
        else
        {
            this->width  = this->texture.subtex->width;
            this->height = this->texture.subtex->height;
        }
    }

    this->InitQuad();

    C3D_TexSetFilter(this->texture.tex, (GPU_TEXTURE_FILTER_PARAM)this->filter.mag, (GPU_TEXTURE_FILTER_PARAM)this->filter.min);
    C3D_TexSetWrap(this->texture.tex, (GPU_TEXTURE_WRAP_PARAM)this->wrap.s, (GPU_TEXTURE_WRAP_PARAM)this->wrap.t);
}

Image::~Image()
{}

void Image::Draw(const DrawArgs & args, const Color & color)
{
    this->Draw(args, this->quad, color);
}

void Image::Draw(const DrawArgs & args, love::Quad * quad, const Color & color)
{
    C2D_DrawParams params;
    Quad::Viewport v = quad->GetViewport();
    Tex3DS_SubTexture sub;

    params.depth = args.depth;
    params.pos =
    {
        args.x, args.y,
        args.scalarX * (float)v.w,
        args.scalarY * (float)v.h
    };
    params.angle = args.r;

    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, color.a);

    /* Set up our Quad to render */

    int w =  quad->GetTextureWidth(true);
    int h =  quad->GetTextureHeight(true);

    sub.top    = 1.0f - (v.y / h);
    sub.left   = (v.x / w);

    sub.right  = (v.x + v.w) / w;
    sub.bottom = 1.0f - ((v.y + v.h) / h);

    sub.width  = v.w;
    sub.height = v.h;

    this->texture.subtex = &sub;

    /* Render the texture */

    C2D_DrawImage(this->texture, &params, &tint);
}
