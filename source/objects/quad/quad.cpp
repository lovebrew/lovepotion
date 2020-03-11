#include "common/runtime.h"
#include "objects/quad/quad.h"

using namespace love;

love::Type Quad::type("Quad", &Object::type);

Quad::Quad(const Viewport & viewport, double sw, double sh) : sw(sw),
                                                              sh(sh)
{
    this->Refresh(viewport, sw, sh);
}

Quad::~Quad()
{}

void Quad::Refresh(const Viewport & viewport, double sw, double sh)
{
    this->viewport = viewport;

    this->sw = sw;
    this->sh = sh;

    this->scaledWidth  = NextPO2(sw);
    this->scaledHeight = NextPO2(sh);
}

double Quad::GetTextureWidth(bool scaled) const
{
    if (scaled)
        return this->scaledWidth;

    return this->sw;
}

double Quad::GetTextureHeight(bool scaled) const
{
    if (scaled)
        return this->scaledHeight;

    return this->sh;
}

void Quad::SetViewport(const Viewport & viewport)
{
    this->Refresh(viewport, this->sw, this->sh);
}

Quad::Viewport Quad::GetViewport() const
{
    return this->viewport;
}
