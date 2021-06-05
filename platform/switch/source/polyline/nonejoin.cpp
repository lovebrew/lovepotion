#include "polyline/nonejoin.h"

void NoneJoinPolyline::CalculateOverdrawVertexCount(bool /* isLooping */)
{
    this->overdrawVertexCount = 4 * (this->vertexCount - 2);
}

void NoneJoinPolyline::FillColorArray(Colorf constant_color, Colorf* colors, int count)
{
    for (int i = 0; i < count; ++i)
    {
        Colorf c = constant_color;
        c.a *= (i & 3) < 2; // if (i % 4 == 2 || i % 4 == 3) c.a = 0
        colors[i] = c;
    }
}

void NoneJoinPolyline::RenderOverdraw(const std::vector<Vector2>& /*normals*/, float pixel_size,
                                      bool /*is_looping*/)
{
    for (size_t i = 2; i + 3 < this->vertexCount; i += 4)
    {
        // v0-v2
        // | / | <- main quad line
        // v1-v3

        Vector2 s = vertices[i + 0] - vertices[i + 2];
        Vector2 t = vertices[i + 0] - vertices[i + 1];
        s.normalize(pixel_size);
        t.normalize(pixel_size);

        const size_t k = 4 * (i - 2);

        overdraw[k + 0] = vertices[i + 0];
        overdraw[k + 1] = vertices[i + 1];
        overdraw[k + 2] = vertices[i + 0] + s + t;
        overdraw[k + 3] = vertices[i + 1] + s - t;

        overdraw[k + 4] = vertices[i + 1];
        overdraw[k + 5] = vertices[i + 3];
        overdraw[k + 6] = vertices[i + 1] + s - t;
        overdraw[k + 7] = vertices[i + 3] - s - t;

        overdraw[k + 8]  = vertices[i + 3];
        overdraw[k + 9]  = vertices[i + 2];
        overdraw[k + 10] = vertices[i + 3] - s - t;
        overdraw[k + 11] = vertices[i + 2] - s + t;

        overdraw[k + 12] = vertices[i + 2];
        overdraw[k + 13] = vertices[i + 0];
        overdraw[k + 14] = vertices[i + 2] - s + t;
        overdraw[k + 15] = vertices[i + 0] + s + t;
    }
}

void NoneJoinPolyline::RenderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                  Vector2& s, float& lengthS, Vector2& normalS, const Vector2& q,
                                  const Vector2& r, float halfWidth)
{
    //   ns1------ns2
    //    |        |
    //    q ------ r
    //    |        |
    // (-ns1)----(-ns2)

    anchors.push_back(q);
    anchors.push_back(q);
    normals.push_back(normalS);
    normals.push_back(-normalS);

    s       = (r - q);
    lengthS = s.getLength();
    normalS = s.getNormal(halfWidth / lengthS);

    anchors.push_back(q);
    anchors.push_back(q);
    normals.push_back(normalS);
    normals.push_back(-normalS);
}
