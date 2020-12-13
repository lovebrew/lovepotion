#include "common/runtime.h"
#include "objects/quad/quad.h"

using namespace love;

Quad::Quad(const Viewport & viewport, double sw, double sh) : common::Quad(sw, sh)
{
    this->Refresh(viewport, sw, sh);
}

const Tex3DS_SubTexture & Quad::GetTex3DSViewport() const
{
    return this->subTex;
}

void Quad::Refresh(const Viewport & viewport, double sw, double sh)
{
    common::Quad::Refresh(viewport, sw, sh);

    int w =  NextPO2(sw);
    int h =  NextPO2(sh);

    Viewport temp = viewport;

    temp.x += 1;
    temp.y += 1;

    temp.w -= 2;
    temp.h -= 2;

    this->subTex.top    = 1.0f - (temp.y / h);
    this->subTex.left   = (temp.x / w);

    this->subTex.right  = (temp.x + temp.w) / w;
    this->subTex.bottom = 1.0f - ((temp.y + temp.h) / h);

    this->subTex.width  = temp.w;
    this->subTex.height = temp.h;
}