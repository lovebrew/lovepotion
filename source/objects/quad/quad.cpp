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

    #if defined (_3DS)
        this->scaledWidth  = NextPO2(sw);
        this->scaledHeight = NextPO2(sh);
    #elif defined (__SWITCH__)
        // Vertices are ordered for use with triangle strips:
        // 0---2
        // | / |
        // 1---3
        this->vertexPositions[0] = Vector2(0.0f, 0.0f);
        this->vertexPositions[1] = Vector2(0.0f, (float)viewport.h);
        this->vertexPositions[2] = Vector2((float)viewport.w, (float)viewport.h);
        this->vertexPositions[3] = Vector2((float)viewport.w, 0.0f);

        this->vertexTexCoords[0] = Vector2((float)(viewport.x / sw), (float)(viewport.y / sh));
        this->vertexTexCoords[1] = Vector2((float)(viewport.x / sw), (float)((viewport.y + viewport.h) / sh));
        this->vertexTexCoords[2] = Vector2((float)((viewport.x + viewport.w) / sw), (float)((viewport.y + viewport.h) / sh));
        this->vertexTexCoords[3] = Vector2((float)((viewport.x + viewport.w) / sw), (float)(viewport.y / sh));
    #endif
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
