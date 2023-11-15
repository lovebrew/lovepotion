/**
 * Copyright (c) 2006-2023 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

// LOVE
#include <utilities/driver/renderer/polyline/polyline.hpp>

#include <modules/graphics_ext.hpp>

// C++
#include <algorithm>

using namespace love;

void Polyline::render(const Vector2* coords, size_t count, size_t size_hint, float halfwidth,
                      float pixel_size, bool draw_overdraw)
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
    Vector2 segment;
    if (!is_looping) // virtual starting point at second point mirrored on first point
        segment = coords[1] - coords[0];
    else // virtual starting point at last vertex
        segment = coords[0] - coords[count - 2];

    float segmentLength   = segment.getLength();
    Vector2 segmentNormal = segment.getNormal(halfwidth / segmentLength);

    Vector2 pointA, pointB(coords[0]);
    for (size_t i = 0; i + 1 < count; i++)
    {
        pointA = pointB;
        pointB = coords[i + 1];
        renderEdge(anchors, normals, segment, segmentLength, segmentNormal, pointA, pointB,
                   halfwidth);
    }

    pointA = pointB;
    pointB = is_looping ? coords[1] : pointB + segment;
    renderEdge(anchors, normals, segment, segmentLength, segmentNormal, pointA, pointB, halfwidth);

    vertex_count = normals.size();

    size_t extra_vertices = 0;

    if (draw_overdraw)
    {
        calc_overdraw_vertex_count(is_looping);

        // When drawing overdraw lines using triangle strips, we want to add an
        // extra degenerate triangle in between the core line and the overdraw
        // line in order to break up the strip into two. This will let us draw
        // everything in one draw call.
        if (triangle_mode == vertex::TRIANGLE_STRIP)
            extra_vertices = 2;
    }

    // Use a single linear array for both the regular and overdraw vertices.
    vertices = new Vector2[vertex_count + extra_vertices + overdraw_vertex_count];

    for (size_t i = 0; i < vertex_count; ++i)
        vertices[i] = anchors[i] + normals[i];

    if (draw_overdraw)
    {
        overdraw              = vertices + vertex_count + extra_vertices;
        overdraw_vertex_start = vertex_count + extra_vertices;
        render_overdraw(normals, pixel_size, is_looping);
    }

    // Add the degenerate triangle strip.
    if (extra_vertices)
    {
        vertices[vertex_count + 0] = vertices[vertex_count - 1];
        vertices[vertex_count + 1] = vertices[overdraw_vertex_start];
    }
}

void Polyline::calc_overdraw_vertex_count(bool is_looping)
{
    overdraw_vertex_count = 2 * vertex_count + (is_looping ? 0 : 2);
}

void Polyline::render_overdraw(const std::vector<Vector2>& normals, float pixel_size,
                               bool is_looping)
{
    // upper segment
    for (size_t i = 0; i + 1 < vertex_count; i += 2)
    {
        overdraw[i]     = vertices[i];
        overdraw[i + 1] = vertices[i] + normals[i] * (pixel_size / normals[i].getLength());
    }
    // lower segment
    for (size_t i = 0; i + 1 < vertex_count; i += 2)
    {
        size_t k                   = vertex_count - i - 1;
        overdraw[vertex_count + i] = vertices[k];
        overdraw[vertex_count + i + 1] =
            vertices[k] + normals[k] * (pixel_size / normals[k].getLength());
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
        overdraw[overdraw_vertex_count - 3] += spacer;

        // right edge
        spacer = (overdraw[vertex_count - 1] - overdraw[vertex_count - 3]);
        spacer.normalize(pixel_size);
        overdraw[vertex_count - 1] += spacer;
        overdraw[vertex_count + 1] += spacer;

        // we need to draw two more triangles to close the
        // overdraw at the line start.
        overdraw[overdraw_vertex_count - 2] = overdraw[0];
        overdraw[overdraw_vertex_count - 1] = overdraw[1];
    }
}

Polyline::~Polyline()
{
    if (vertices)
        delete[] vertices;
}

void Polyline::draw(Graphics<Console::ALL>* gfx)
{
    const auto& t  = gfx->GetTransform();
    bool is2D      = t.IsAffine2DTransform();
    Color curcolor = gfx->GetColor();

    int overdraw_start = (int)overdraw_vertex_start;
    int overdraw_count = (int)overdraw_vertex_count;

    int total_vertex_count = (int)vertex_count;
    if (overdraw)
        total_vertex_count = overdraw_start + overdraw_count;

    // love's automatic batching can only deal with < 65k vertices per draw.
    // uint16_max - 3 is evenly divisible by 6 (needed for quads mode).
    int maxvertices = LOVE_UINT16_MAX - 3;

    int advance = maxvertices;
    if (triangle_mode == vertex::TRIANGLE_STRIP)
        advance -= 2;

    for (int vertex_start = 0; vertex_start < total_vertex_count; vertex_start += advance)
    {
        const Vector2* verts = vertices + vertex_start;

        int totalVertices = std::min(maxvertices, total_vertex_count - vertex_start);

        auto mode = vertex::PRIMITIVE_TRIANGLE_STRIP;
        if (this->triangle_mode == vertex::TRIANGLE_QUADS)
            mode = vertex::PRIMITIVE_QUADS;

        DrawCommand command(totalVertices, mode);

        if (is2D)
            t.TransformXY(std::span(command.Positions().get(), totalVertices),
                          std::span(verts, totalVertices));

        Color colordata[totalVertices] {};

        int draw_rough_count = std::min((int)command.count, (int)vertex_count - vertex_start);

        // Constant vertex color up to the overdraw vertices.
        for (int i = 0; i < draw_rough_count; i++)
            colordata[i] = curcolor;

        if (overdraw)
        {
            int draw_remaining_count = command.count - draw_rough_count;

            int draw_overdraw_begin = overdraw_start - vertex_start;
            int draw_overdraw_end   = draw_overdraw_begin + overdraw_count;

            draw_overdraw_begin = std::max(0, draw_overdraw_begin);

            int draw_overdraw_count =
                std::min(draw_remaining_count, draw_overdraw_end - draw_overdraw_begin);

            if (draw_overdraw_count > 0)
            {
                Color* colors = colordata + draw_overdraw_begin;
                fill_color_array(curcolor, colors, draw_overdraw_count);
            }
        }

        command.FillVertices(colordata);

        Renderer<Console::Which>::Instance().Render(command);
    }
}

void Polyline::fill_color_array(Color constant_color, Color* colors, int count)
{
    for (int i = 0; i < count; ++i)
    {
        Color c = constant_color;
        c.a *= (i + 1) % 2; // avoids branching. equiv to if (i%2 == 1) c.a = 0;
        colors[i] = c;
    }
}
