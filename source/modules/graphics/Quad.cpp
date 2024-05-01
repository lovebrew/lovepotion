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

    void Quad::refresh(const Viewport& v, double sourceWidth, double sourceHeight)
    {
        this->viewport     = v;
        this->sourceWidth  = sourceWidth;
        this->sourceHeight = sourceHeight;

        this->vertexPositions[0] = Vector2(0.0f, 0.0f);
        this->vertexPositions[1] = Vector2(0.0f, (float)v.h);
        this->vertexPositions[2] = Vector2((float)v.w, 0.0f);
        this->vertexPositions[3] = Vector2((float)v.w, (float)v.h);

        // clang-format off
        this->textureCoordinates[0] = Vector2(float(v.x / sourceWidth), float(v.y / sourceHeight));
        this->textureCoordinates[1] = Vector2(float(v.x / sourceWidth), float((v.y + v.h) / sourceHeight));
        this->textureCoordinates[2] = Vector2(float((v.x + v.w) / sourceWidth), float(v.y / sourceHeight));
        this->textureCoordinates[3] = Vector2(float((v.x + v.w) / sourceWidth), float((v.y + v.h) / sourceHeight));
        // clang-format on
    }
} // namespace love
