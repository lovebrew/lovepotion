#include "deko3d/vertex.h"
#include "objects/font/font.h"

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

std::vector<vertex::Vertex> vertex::GenerateTextureFromGlyphs(const GlyphVertex * gVerts, size_t count, const Colorf * colors, size_t colorCount)
{
    std::vector<vertex::Vertex> verts(count);
    Colorf currentColor = colors[0];

    for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
    {
        const GlyphVertex vertex = gVerts[currentVertex];

        if (currentVertex < colorCount)
            currentColor = colors[currentVertex];

        vertex::Vertex vert =
        {
            .position = {vertex.x, vertex.y, 0.0f},
            .color = {1, 1, 1, 1},
            .texcoord = {vertex.s, vertex.t}
        };

        currentColor.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}

void vertex::DebugVertex(const vertex::Vertex & vertex)
{
    char buffer[255];
    snprintf(buffer, sizeof(buffer), "x: %.2f y: %.2f z: %.2f / color: {%.2f, %.2f, %.2f, %.2f} / texCoord {%.2f, %.2f}",
                                      vertex.position[0], vertex.position[1], vertex.position[2], vertex.color[0], vertex.color[1],
                                      vertex.color[2], vertex.color[3], vertex.texcoord[0], vertex.texcoord[1]);
    LOG("%s", buffer);
}