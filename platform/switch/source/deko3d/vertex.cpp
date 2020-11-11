#include "deko3d/vertex.h"

using namespace love;

std::vector<vertex::Vertex> vertex::GeneratePrimitiveFromVectors(const Vector2 * points, size_t count, const Colorf * colors, size_t colorCount)
{
    std::vector<vertex::Vertex> verts(count);
    Colorf currentColor = colors[0];

    for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
    {
        const Vector2 point = points[currentVertex];

        if (currentVertex < colorCount)
            currentColor = colors[currentVertex];

        vertex::Vertex vert =
        {
            .position = {point.x, point.y, 0.0f},
            .color = {1, 1, 1, 1},
            .texcoord = {0, 0}
        };

        currentColor.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}

std::vector<vertex::Vertex> vertex::GenerateTextureFromVectors(const love::Vector2 * points, const love::Vector2 * texcoord, size_t count, Colorf color)
{
    std::vector<vertex::Vertex> verts(count);

    for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
    {
        const Vector2 point    = points[currentVertex];
        const Vector2 texCoord = texcoord[currentVertex];

        vertex::Vertex vert =
        {
            .position = {point.x, point.y, 0.0f},
            .color = {1, 1, 1, 1},
            .texcoord = {texCoord.x, texCoord.y}
        };

        color.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}