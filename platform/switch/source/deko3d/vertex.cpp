#include "deko3d/vertex.h"
#include "objects/font/font.h"

using namespace love;

std::vector<vertex::PrimitiveVertex> vertex::GenerateTextureFromVectors(
    const love::Vector2* points, const love::Vector2* texcoord, size_t count, Colorf color)
{
    std::vector<vertex::PrimitiveVertex> verts(count);

    for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
    {
        const Vector2 point    = points[currentVertex];
        const Vector2 texCoord = texcoord[currentVertex];

        vertex::PrimitiveVertex vert = { .position = { point.x, point.y, 0.0f },
                                         .color    = { 1, 1, 1, 1 },
                                         .texcoord = { normto16t(texCoord.x),
                                                       normto16t(texCoord.y) } };

        color.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}

std::vector<vertex::PrimitiveVertex> vertex::GenerateTextureFromGlyphs(
    const vertex::GlyphVertex* data, size_t count)
{
    std::vector<vertex::PrimitiveVertex> verts(count);

    for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
    {
        const GlyphVertex vertex = data[currentVertex];
        Colorf color             = vertex.color;

        vertex::PrimitiveVertex vert = { .position = { vertex.x, vertex.y, 0.0f },
                                         .color    = { 1, 1, 1, 1 },
                                         .texcoord = { vertex.s, vertex.t } };

        color.CopyTo(vert.color);

        verts[currentVertex] = vert;
    }

    return verts;
}
