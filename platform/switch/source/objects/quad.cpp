#include "common/runtime.h"
#include "objects/quad/quad.h"

using namespace love;

Quad::Quad(const Viewport & viewport, double sw, double sh) : common::Quad(sw, sh)
{
    this->Refresh(viewport, sw, sh);
}

void Quad::Refresh(const Viewport & viewport, double sw, double sh)
{
    common::Quad::Refresh(viewport, sw, sh);

    this->vertexPositions[0] = Vector2(0.0f, 0.0f);
    this->vertexPositions[1] = Vector2(0.0f, (float)viewport.h);
    this->vertexPositions[2] = Vector2((float)viewport.w, (float)viewport.h);
    this->vertexPositions[3] = Vector2((float)viewport.w, 0.0f);

    this->vertexTexCoords[0] = Vector2((float)(viewport.x / sw), (float)(viewport.y / sh));
    this->vertexTexCoords[1] = Vector2((float)(viewport.x / sw), (float)((viewport.y + viewport.h) / sh));
    this->vertexTexCoords[2] = Vector2((float)((viewport.x + viewport.w) / sw), (float)((viewport.y + viewport.h) / sh));
    this->vertexTexCoords[3] = Vector2((float)((viewport.x + viewport.w) / sw), (float)(viewport.y / sh));
}