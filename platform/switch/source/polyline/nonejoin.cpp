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

void NoneJoinPolyline::RenderOverdraw(const std::vector<Vector2>& /*normals*/, float pixelSize,
                                      bool /*is_looping*/)
{
    for (size_t i = 2; i + 3 < this->vertexCount; i += 4)
    {
        // v0-v2
        // | / | <- main quad line
        // v1-v3

        Vector2 s = this->vertices[i + 0] - this->vertices[i + 2];
        Vector2 t = this->vertices[i + 0] - this->vertices[i + 1];

        s.normalize(pixelSize);
        t.normalize(pixelSize);

        const size_t k = 4 * (i - 2);

        this->overdraw[k + 0] = this->vertices[i + 0];
        this->overdraw[k + 1] = this->vertices[i + 1];
        this->overdraw[k + 2] = this->vertices[i + 0] + s + t;
        this->overdraw[k + 3] = this->vertices[i + 1] + s - t;

        this->overdraw[k + 4] = this->vertices[i + 1];
        this->overdraw[k + 5] = this->vertices[i + 3];
        this->overdraw[k + 6] = this->vertices[i + 1] + s - t;
        this->overdraw[k + 7] = this->vertices[i + 3] - s - t;

        this->overdraw[k + 8]  = this->vertices[i + 3];
        this->overdraw[k + 9]  = this->vertices[i + 2];
        this->overdraw[k + 10] = this->vertices[i + 3] - s - t;
        this->overdraw[k + 11] = this->vertices[i + 2] - s + t;

        this->overdraw[k + 12] = this->vertices[i + 2];
        this->overdraw[k + 13] = this->vertices[i + 0];
        this->overdraw[k + 14] = this->vertices[i + 2] - s + t;
        this->overdraw[k + 15] = this->vertices[i + 0] + s + t;
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
    normals.push_back(-normalS);
    normals.push_back(normalS);
}
