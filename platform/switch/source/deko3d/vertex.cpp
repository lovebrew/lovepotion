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

static inline uint16_t normto16t(float in)
{ return uint16_t(in * 0xFFFF); }

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
            .texcoord = {normto16t(texCoord.x), normto16t(texCoord.y)}
        };

        color.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}

std::vector<vertex::Vertex> vertex::GenerateTextureFromGlyphs(const vertex::GlyphVertex * data, size_t count)
{
    std::vector<vertex::Vertex> verts(count);

    for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
    {
        const GlyphVertex vertex = data[currentVertex];
        Colorf color = vertex.color;

        vertex::Vertex vert =
        {
            .position = {vertex.x, vertex.y, 0.0f},
            .color = {1, 1, 1, 1},
            .texcoord = {vertex.s, vertex.t}
        };

        color.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}
