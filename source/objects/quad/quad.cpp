#include <objects/quad/quad.hpp>

using namespace love;

Type Quad::type("Quad", &Object::type);

Quad::Quad(const Viewport& viewport, double sourceWidth, double sourceHeight) :
    sourceWidth(sourceWidth),
    sourceHeight(sourceHeight)
{
    this->arrayLayer = 0;
    this->Refresh(viewport, sourceWidth, sourceHeight);
}

Quad::~Quad()
{}

void Quad::Refresh(const Viewport& viewport, double sourceWidth, double sourceHeight)
{
    this->viewport     = viewport;
    this->sourceWidth  = sourceWidth;
    this->sourceHeight = sourceHeight;

    std::fill_n(this->vertexPositions, 4, Vector2 {});

    this->vertexPositions[0] = Vector2(0.0f, 0.0f);
    this->vertexPositions[1] = Vector2(0.0f, (float)viewport.h);
    this->vertexPositions[2] = Vector2((float)viewport.w, (float)viewport.h);
    this->vertexPositions[3] = Vector2((float)viewport.w, 0.0f);

    std::fill_n(this->vertexTextureCoords, 4, Vector2 {});

    // clang-format off
    this->vertexTextureCoords[0] = Vector2((float)(viewport.x / sourceWidth), (float)(viewport.y / sourceHeight));
    this->vertexTextureCoords[1] = Vector2((float)(viewport.x / sourceWidth), (float)((viewport.y + viewport.h) / sourceHeight));
    this->vertexTextureCoords[2] = Vector2((float)((viewport.x + viewport.w) / sourceWidth), (float)((viewport.y + viewport.h) / sourceHeight));
    this->vertexTextureCoords[3] = Vector2((float)((viewport.x + viewport.w) / sourceWidth), (float)(viewport.y / sourceHeight));
    // clang-format on
}

void Quad::SetViewport(const Viewport& viewport)
{
    this->Refresh(viewport, sourceWidth, sourceHeight);
}

Quad::Viewport Quad::GetViewport() const
{
    return this->viewport;
}

double Quad::GetTextureWidth() const
{
    return this->sourceWidth;
}

double Quad::GetTextureHeight() const
{
    return this->sourceHeight;
}

const Vector2* Quad::GetVertexPositions() const
{
    return this->vertexPositions;
}

const Vector2* Quad::GetVertexTextureCoords() const
{
    return this->vertexTextureCoords;
}

void Quad::SetLayer(int layer)
{
    this->arrayLayer = layer;
}

int Quad::GetLayer() const
{
    return this->arrayLayer;
}
