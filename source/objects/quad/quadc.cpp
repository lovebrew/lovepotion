#include "objects/quad/quadc.h"

using namespace love::common;

love::Type Quad::type("Quad", &Object::type);

Quad::Quad(double sw, double sh) : sw(sw), sh(sh)
{}

Quad::~Quad()
{}

const Quad::Viewport& Quad::GetViewport() const
{
    return this->viewport;
}

double Quad::GetTextureWidth() const
{
    return this->sw;
}

double Quad::GetTextureHeight() const
{
    return this->sh;
}

void Quad::RefreshViewport(const Viewport& viewport, double sw, double sh)
{
    this->viewport = viewport;

    this->sw = sw;
    this->sh = sh;
}

void Quad::SetViewport(const Viewport& viewport)
{
    this->Refresh(viewport, this->sw, this->sh);
}
