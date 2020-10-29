#include "common/runtime.h"
#include "deko3d/polyline.h"

#include "modules/graphics/graphics.h"

using namespace love;

constexpr float LINES_PARALLEL_EPS = 0.05f;

void Polyline::Render(const Vector2 * coords, size_t count, size_t size_hint, float halfwidth, float pixel_size, bool draw_overdraw)
{
    static std::vector<Vector2> anchors;
    anchors.clear();
    anchors.reserve(size_hint);

    static std::vector<Vector2> normals;
    normals.clear();
    normals.reserve(size_hint);

    // prepare vertex arrays
    if (draw_overdraw)
        halfwidth -= pixel_size * 0.3f;

    // compute sleeve
    bool is_looping = (coords[0] == coords[count - 1]);

    Vector2 s;
    if (!is_looping) // virtual starting point at second point mirrored on first point
        s = coords[1] - coords[0];
    else // virtual starting point at last vertex
        s = coords[0] - coords[count - 2];

    float len_s = s.getLength();
    Vector2 ns = s.getNormal(halfwidth / len_s);

    Vector2 q, r(coords[0]);
    for (size_t i = 0; i + 1 < count; i++)
    {
        q = r;
        r = coords[i + 1];
        this->RenderEdge(anchors, normals, s, len_s, ns, q, r, halfwidth);
    }

    q = r;
    r = is_looping ? coords[1] : r + s;
    this->RenderEdge(anchors, normals, s, len_s, ns, q, r, halfwidth);

    this->vertexCount = normals.size();

    size_t extra_vertices = 0;

    if (draw_overdraw)
    {
        this->CalculateOverdrawVertexCount(is_looping);

        // When drawing overdraw lines using triangle strips, we want to add an
        // extra degenerate triangle in between the core line and the overdraw
        // line in order to break up the strip into two. This will let us draw
        // everything in one draw call.
        if (this->triangleMode == DkPrimitive_TriangleStrip)
            extra_vertices = 2;
    }

    // Use a single linear array for both the regular and overdraw vertices.
    vertices = new Vector2[this->vertexCount + extra_vertices + this->overdrawCount];

    for (size_t i = 0; i < this->vertexCount; ++i)
        vertices[i] = anchors[i] + normals[i];

    if (draw_overdraw)
    {
        overdraw = vertices + this->vertexCount + extra_vertices;
        this->overdrawCount = this->vertexCount + extra_vertices;
        this->RenderOverdraw(normals, pixel_size, is_looping);
    }

    // Add the degenerate triangle strip.
    if (extra_vertices)
    {
        vertices[this->vertexCount + 0] = vertices[this->vertexCount - 1];
        vertices[this->vertexCount + 1] = vertices[this->overdrawStart];
    }
}

void Polyline::Draw(love::Graphics * gfx)
{
    // const Matrix4 &t = gfx->getTransform();
    // bool is2D = t.isAffine2DTransform();
    // Color32 curcolor = toColor32(gfx->getColor());

    int overdraw_start = (int)this->overdrawStart;
    int overdraw_count = (int)this->overdrawCount;

    int total_vertex_count = (int)this->vertexCount;

    Colorf curColor = gfx->GetColor();

    if (overdraw)
        total_vertex_count = overdraw_start + overdraw_count;

    // love's automatic batching can only deal with < 32k vertices per draw.
    // uint16_max - 3 is evenly divisible by 6 (needed for quads mode).
    int maxvertices = std::numeric_limits<int16_t>::max() - 3;

    int advance = maxvertices;
    if (this->triangleMode == DkPrimitive_TriangleStrip)
        advance -= 2;

    for (int vertex_start = 0; vertex_start < total_vertex_count; vertex_start += advance)
    {
        const Vector2 * verts = vertices + vertex_start;
        int vtxCount = std::min(maxvertices, total_vertex_count - vertex_start);

        int draw_rough_count = std::min(vtxCount, (int)this->vertexCount - vertex_start);

        Colorf color[draw_rough_count] = { gfx->GetColor() };

        std::vector<vertex::Vertex> renderVerts = vertex::GenerateFromVectors(verts, vtxCount, color, vtxCount);

        // Constant vertex color up to the overdraw vertices.
        for (int i = 0; i < draw_rough_count; i++)
            curColor.CopyTo(renderVerts[i].color);

        dk3d.RenderPolyline(this->triangleMode, renderVerts.data(), vtxCount * sizeof(*renderVerts.data()), vtxCount);

        if (overdraw)
        {
            int draw_remaining_count = vtxCount - draw_rough_count;

            int draw_overdraw_begin = overdraw_start - vertex_start;
            int draw_overdraw_end = draw_overdraw_begin + overdraw_count;

            draw_overdraw_begin = std::max(0, draw_overdraw_begin);

            int draw_overdraw_count = std::min(draw_remaining_count, draw_overdraw_end - draw_overdraw_begin);

            if (draw_overdraw_count > 0)
            {
                Colorf * colors = color + draw_overdraw_begin;
                this->FillColorArray(curColor, colors, draw_overdraw_count);
            }
        }
    }

}

void Polyline::CalculateOverdrawVertexCount(bool is_looping)
{
    this->overdrawCount = 2 * this->vertexCount + (is_looping ? 0 : 2);
}

void Polyline::RenderOverdraw(const std::vector<Vector2> &normals, float pixel_size, bool is_looping)
{
    // upper segment
    for (size_t i = 0; i + 1 < this->vertexCount; i += 2)
    {
        overdraw[i]   = vertices[i];
        overdraw[i+1] = vertices[i] + normals[i] * (pixel_size / normals[i].getLength());
    }
    // lower segment
    for (size_t i = 0; i + 1 < vertexCount; i += 2)
    {
        size_t k = vertexCount - i - 1;
        overdraw[this->vertexCount + i]   = vertices[k];
        overdraw[this->vertexCount + i+1] = vertices[k] + normals[k] * (pixel_size / normals[k].getLength());
    }

    // if not looping, the outer overdraw vertices need to be displaced
    // to cover the line endings, i.e.:
    // +- - - - //- - +         +- - - - - //- - - +
    // +-------//-----+         : +-------//-----+ :
    // | core // line |   -->   : | core // line | :
    // +-----//-------+         : +-----//-------+ :
    // +- - //- - - - +         +- - - //- - - - - +
    if (!is_looping)
    {
        // left edge
        Vector2 spacer = (overdraw[1] - overdraw[3]);
        spacer.normalize(pixel_size);
        overdraw[1] += spacer;
        overdraw[this->overdrawCount - 3] += spacer;

        // right edge
        spacer = (overdraw[vertexCount - 1] - overdraw[this->vertexCount - 3]);
        spacer.normalize(pixel_size);
        overdraw[this->vertexCount - 1] += spacer;
        overdraw[this->vertexCount + 1] += spacer;

        // we need to draw two more triangles to close the
        // overdraw at the line start.
        overdraw[this->overdrawCount - 2] = overdraw[0];
        overdraw[this->overdrawCount - 1] = overdraw[1];
    }
}

void NoneJoinPolyline::CalculateOverdrawVertexCount(bool /*is_looping*/)
{
    this->overdrawCount = 4 * (this->vertexCount - 2); // less than ideal
}

void NoneJoinPolyline::RenderOverdraw(const std::vector<Vector2> &/*normals*/, float pixel_size, bool /*is_looping*/)
{
    for (size_t i = 2; i + 3 < this->vertexCount; i += 4)
    {
        // v0-v2
        // | / | <- main quad line
        // v1-v3

        Vector2 s = vertices[i+0] - vertices[i+2];
        Vector2 t = vertices[i+0] - vertices[i+1];

        s.normalize(pixel_size);
        t.normalize(pixel_size);

        const size_t k = 4 * (i - 2);

        overdraw[k+0] = vertices[i+0];
        overdraw[k+1] = vertices[i+1];
        overdraw[k+2] = vertices[i+0] + s + t;
        overdraw[k+3] = vertices[i+1] + s - t;

        overdraw[k+4] = vertices[i+1];
        overdraw[k+5] = vertices[i+3];
        overdraw[k+6] = vertices[i+1] + s - t;
        overdraw[k+7] = vertices[i+3] - s - t;

        overdraw[k+ 8] = vertices[i+3];
        overdraw[k+ 9] = vertices[i+2];
        overdraw[k+10] = vertices[i+3] - s - t;
        overdraw[k+11] = vertices[i+2] - s + t;

        overdraw[k+12] = vertices[i+2];
        overdraw[k+13] = vertices[i+0];
        overdraw[k+14] = vertices[i+2] - s + t;
        overdraw[k+15] = vertices[i+0] + s + t;
    }
}

Polyline::~Polyline()
{
    if (vertices)
        delete[] vertices;
}

void Polyline::FillColorArray(Colorf constant_color, Colorf * colors, int count)
{
    for (int i = 0; i < count; ++i)
    {
        Colorf c = constant_color;
        c.a *= (i+1) % 2; // avoids branching. equiv to if (i%2 == 1) c.a = 0;
        colors[i] = c;
    }
}

void NoneJoinPolyline::FillColorArray(Colorf constant_color, Colorf * colors, int count)
{
    for (int i = 0; i < count; ++i)
    {
        Colorf c = constant_color;
        c.a *= (i & 3) < 2; // if (i % 4 == 2 || i % 4 == 3) c.a = 0
        colors[i] = c;
    }
}

void NoneJoinPolyline::RenderEdge(std::vector<Vector2> &anchors, std::vector<Vector2> &normals,
                                  Vector2 &s, float &len_s, Vector2 &ns,
                                  const Vector2 &q, const Vector2 &r, float hw)
{
    //   ns1------ns2
    //    |        |
    //    q ------ r
    //    |        |
    // (-ns1)----(-ns2)

    anchors.push_back(q);
    anchors.push_back(q);
    normals.push_back(ns);
    normals.push_back(-ns);

    s     = (r - q);
    len_s = s.getLength();
    ns    = s.getNormal(hw / len_s);

    anchors.push_back(q);
    anchors.push_back(q);
    normals.push_back(ns);
    normals.push_back(-ns);
}

void MiterJoinPolyline::RenderEdge(std::vector<Vector2> &anchors, std::vector<Vector2> &normals,
                                   Vector2 &s, float &len_s, Vector2 &ns,
                                   const Vector2 &q, const Vector2 &r, float hw)
{
    Vector2 t    = (r - q);
    float len_t = t.getLength();
    Vector2 nt   = t.getNormal(hw / len_t);

    anchors.push_back(q);
    anchors.push_back(q);

    float det = Vector2::cross(s, t);
    if (fabs(det) / (len_s * len_t) < LINES_PARALLEL_EPS && Vector2::dot(s, t) > 0)
    {
        // lines parallel, compute as u1 = q + ns * w/2, u2 = q - ns * w/2
        normals.push_back(ns);
        normals.push_back(-ns);
    }
    else
    {
        // cramers rule
        float lambda = Vector2::cross((nt - ns), t) / det;
        Vector2 d = ns + s * lambda;
        normals.push_back(d);
        normals.push_back(-d);
    }

    s     = t;
    ns    = nt;
    len_s = len_t;
}

void BevelJoinPolyline::RenderEdge(std::vector<Vector2> &anchors, std::vector<Vector2> &normals,
                                   Vector2 &s, float &len_s, Vector2 &ns,
                                   const Vector2 &q, const Vector2 &r, float hw)
{
    Vector2 t    = (r - q);
    float len_t = t.getLength();

    float det = Vector2::cross(s, t);
    if (fabs(det) / (len_s * len_t) < LINES_PARALLEL_EPS && Vector2::dot(s, t) > 0)
    {
        // lines parallel, compute as u1 = q + ns * w/2, u2 = q - ns * w/2
        Vector2 n = t.getNormal(hw / len_t);
        anchors.push_back(q);
        anchors.push_back(q);
        normals.push_back(n);
        normals.push_back(-n);
        s     = t;
        len_s = len_t;
        return; // early out
    }

    // cramers rule
    Vector2 nt = t.getNormal(hw / len_t);
    float lambda = Vector2::cross((nt - ns), t) / det;
    Vector2 d = ns + s * lambda;

    anchors.push_back(q);
    anchors.push_back(q);
    anchors.push_back(q);
    anchors.push_back(q);
    if (det > 0) // 'left' turn -> intersection on the top
    {
        normals.push_back(d);
        normals.push_back(-ns);
        normals.push_back(d);
        normals.push_back(-nt);
    }
    else
    {
        normals.push_back(ns);
        normals.push_back(-d);
        normals.push_back(nt);
        normals.push_back(-d);
    }
    s     = t;
    len_s = len_t;
    ns    = nt;
}