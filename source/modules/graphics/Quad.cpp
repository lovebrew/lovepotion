#include "modules/graphics/Quad.hpp"

#include <cstring>

namespace love
{
    Type Quad::type("Quad", &Object::type);

    Quad::Quad(const Viewport& viewport, double sourceWidth, double sourceHeight) :
        sourceWidth(sourceWidth),
        sourceHeight(sourceHeight)
    {
        this->refresh(viewport, sourceWidth, sourceHeight);
    }

    Quad::~Quad()
    {}

    void Quad::refresh(const Viewport& viewport, double sourceWidth, double sourceHeight)
    {
        this->viewport     = viewport;
        this->sourceWidth  = sourceWidth;
        this->sourceHeight = sourceHeight;

        this->vertexPositions[0] = Vector2(0.0f, 0.0f);
        this->vertexPositions[1] = Vector2(0.0f, viewport.h);
        this->vertexPositions[2] = Vector2(viewport.w, viewport.h);
        this->vertexPositions[3] = Vector2(viewport.w, 0.0f);

        // clang-format off
        this->textureCoordinates[0] = Vector2(viewport.x / sourceWidth, viewport.y / sourceHeight);
        this->textureCoordinates[1] = Vector2(viewport.x / sourceWidth, (viewport.y + viewport.h) / sourceHeight);
        this->textureCoordinates[2] = Vector2((viewport.x + viewport.w) / sourceWidth, (viewport.y + viewport.h) / sourceHeight);
        this->textureCoordinates[3] = Vector2((viewport.x + viewport.w) / sourceWidth, viewport.y / sourceHeight);
        // clang-format on
    }
} // namespace love
