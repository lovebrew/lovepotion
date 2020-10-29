#include "deko3d/vertex.h"

using namespace love;

std::vector<vertex::Vertex> vertex::GenerateFromVectors(const Vector2 * points, size_t count, const Colorf * colors, size_t colorCount)
{
    std::vector<vertex::Vertex> verts(count);
    Colorf currentColor = colors[0];

    for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
    {
        const Vector2 point = points[currentVertex];

        if (currentVertex < colorCount)
            currentColor = colors[currentVertex];

        vertex::Vertex vert = { {point.x, point.y, 0.0f}, {1, 1, 1, 1} };
        currentColor.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}