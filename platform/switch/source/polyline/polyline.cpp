#include "polyline/polyline.h"
#include "modules/graphics/graphics.h"

#include "deko3d/deko.h"

#include <algorithm>

using namespace love;

Polyline::~Polyline()
{
    if (this->vertices)
        delete[] this->vertices;
}

void Polyline::CalculateOverdrawVertexCount(bool isLooping)
{
    this->overdrawVertexCount = 2 * this->vertexCount + (isLooping ? 0 : 2);
}

void Polyline::FillColorArray(Colorf constantColor, Colorf* colors, int count)
{
    for (int i = 0; i < count; ++i)
    {
        Colorf color = constantColor;
        color.a *= (i + 1) % 2; // avoids branching. equiv to if (i%2 == 1) c.a = 0;

        colors[i] = color;
    }
}

void Polyline::Render(const Vector2* coords, size_t count, size_t sizeHint, float halfWidth,
                      float pixelSize, bool drawOverdraw)
{
    static std::vector<Vector2> anchors;
    anchors.clear();
    anchors.reserve(sizeHint);

    static std::vector<Vector2> normals;
    normals.clear();
    normals.reserve(sizeHint);

    /* prepare vertex arrays */
    if (drawOverdraw)
        halfWidth -= pixelSize * 0.3f;

    /* compute sleeve */
    bool isLooping = (coords[0] == coords[count - 1]);

    Vector2 s;
    if (!isLooping) /* virtual starting point at second point mirrored on first point */
        s = coords[1] - coords[0];
    else /* virtual starting point at last vertex */
        s = coords[0] - coords[count - 2];

    float lenS = s.getLength();
    Vector2 nS = s.getNormal(halfWidth / lenS);

    Vector2 q, r(coords[0]);
    for (size_t i = 0; i + 1 < count; i++)
    {
        q = r;
        r = coords[i + 1];

        this->RenderEdge(anchors, normals, s, lenS, nS, q, r, halfWidth);
    }

    q = r;
    r = isLooping ? coords[1] : r + s;
    this->RenderEdge(anchors, normals, s, lenS, nS, q, r, halfWidth);

    /* set vertex count */
    this->vertexCount = normals.size();

    size_t extraVertices = 0;
    if (drawOverdraw)
    {
        this->CalculateOverdrawVertexCount(isLooping);

        if (this->triangleIndexMode == vertex::TriangleIndexMode::STRIP)
            extraVertices = 2;
    }

    this->vertices = new Vector2[this->vertexCount + extraVertices + this->overdrawVertexCount];

    for (size_t i = 0; i < this->vertexCount; ++i)
        this->vertices[i] = anchors[i] + normals[i];

    if (drawOverdraw)
    {
        this->overdraw            = this->vertices + vertexCount + extraVertices;
        this->overdrawVertexStart = this->vertexCount + extraVertices;

        this->RenderOverdraw(normals, pixelSize, isLooping);
    }

    if (extraVertices)
    {
        this->vertices[vertexCount + 0] = this->vertices[this->vertexCount - 1];
        this->vertices[vertexCount + 1] = this->vertices[this->overdrawVertexStart];
    }
}

void Polyline::RenderOverdraw(const std::vector<Vector2>& normals, float pixelSize, bool isLooping)
{
    // upper segment
    for (size_t i = 0; i + 1 < this->vertexCount; i += 2)
    {
        this->overdraw[i] = this->vertices[i];
        this->overdraw[i + 1] =
            this->vertices[i] + normals[i] * (pixelSize / normals[i].getLength());
    }

    // lower segment
    for (size_t i = 0; i + 1 < this->vertexCount; i += 2)
    {
        size_t k = this->vertexCount - i - 1;

        this->overdraw[this->vertexCount + i] = vertices[k];
        this->overdraw[this->vertexCount + i + 1] =
            vertices[k] + normals[k] * (pixelSize / normals[k].getLength());
    }

    // if not looping, the outer overdraw vertices need to be displaced
    // to cover the line endings, i.e.:
    // +- - - - //- - +         +- - - - - //- - - +
    // +-------//-----+         : +-------//-----+ :
    // | core // line |   -->   : | core // line | :
    // +-----//-------+         : +-----//-------+ :
    // +- - //- - - - +         +- - - //- - - - - +
    if (!isLooping)
    {
        // left edge
        Vector2 spacer = (this->overdraw[1] - this->overdraw[3]);
        spacer.normalize(pixelSize);
        this->overdraw[1] += spacer;
        this->overdraw[this->overdrawVertexCount - 3] += spacer;

        // right edge
        spacer = (this->overdraw[this->vertexCount - 1] - this->overdraw[this->vertexCount - 3]);
        spacer.normalize(pixelSize);

        this->overdraw[this->vertexCount - 1] += spacer;
        this->overdraw[this->vertexCount + 1] += spacer;

        // we need to draw two more triangles to close the
        // overdraw at the line start.
        this->overdraw[this->overdrawVertexCount - 2] = this->overdraw[0];
        this->overdraw[this->overdrawVertexCount - 1] = this->overdraw[1];
    }
}

void Polyline::Draw(Graphics* graphics)
{
    const Matrix4& t = graphics->GetTransform();
    bool is2D        = t.IsAffine2DTransform();

    Colorf currentColor = graphics->GetColor();

    int overdrawStart = (int)this->overdrawVertexStart;
    int overdrawCount = (int)this->overdrawVertexCount;

    int totalVertexCount = (int)this->vertexCount;

    if (this->overdraw)
        totalVertexCount = overdrawStart + overdrawCount;

    int maxVertices = std::numeric_limits<uint16_t>::max() - 3;
    int advance     = maxVertices;

    if (this->triangleIndexMode == vertex::TriangleIndexMode::STRIP)
        advance -= 2;

    for (int vertexStart = 0; vertexStart < totalVertexCount; vertexStart += advance)
    {
        const Vector2* verts = this->vertices + vertexStart;
        int cmdVertexCount   = std::min(maxVertices, totalVertexCount - vertexStart);

        /* make vector2 array - size to cmd.vertexCount */
        Vector2 transformed[cmdVertexCount];
        std::fill_n(transformed, cmdVertexCount, Vector2 {});

        if (is2D)
            t.TransformXY(transformed, verts, cmdVertexCount);

        /* make colorf array - size to cmd.vertexCount */
        Colorf colors[cmdVertexCount];
        std::fill_n(colors, cmdVertexCount, Colorf {});

        int drawRoughCount = std::min(cmdVertexCount, (int)this->vertexCount - vertexStart);

        /* Constant vertex color up to the overdraw vertices. */
        for (int i = 0; i < drawRoughCount; i++)
            colors[i] = currentColor;

        if (this->overdraw)
        {
            int drawRemainingCount = cmdVertexCount - drawRoughCount;

            int drawOverdrawBegin = overdrawStart - vertexStart;
            int drawOverdrawEnd   = drawOverdrawBegin + overdrawCount;

            drawOverdrawBegin = std::max(0, drawOverdrawBegin);

            int drawOverdrawCount =
                std::min(drawRemainingCount, drawOverdrawEnd - drawOverdrawBegin);

            if (drawOverdrawCount > 0)
            {
                Colorf* colordata = colors + drawOverdrawBegin;
                this->FillColorArray(currentColor, colordata, drawOverdrawCount);
            }
        }

        auto render = vertex::GeneratePrimitiveFromVectors(std::span(transformed, cmdVertexCount),
                                                           std::span(colors, cmdVertexCount));

        ::deko3d::Instance().RenderPolyline(this->triangleMode, render.get(), cmdVertexCount);
    }
}
