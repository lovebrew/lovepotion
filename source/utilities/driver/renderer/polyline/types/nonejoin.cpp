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
#include <common/vector.hpp>

#include <utilities/driver/renderer/polyline/types/nonejoin.hpp>

#include <modules/graphics_ext.hpp>

using namespace love;

void NoneJoinPolyline::renderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                  Vector2& segment, float& segmentLength, Vector2& segmentNormal,
                                  const Vector2& pointA, const Vector2& pointB, float halfWidth)
{
    //   ns1------ns2
    //    |        |
    //    q ------ r
    //    |        |
    // (-ns1)----(-ns2)

    anchors.push_back(pointA);
    anchors.push_back(pointA);
    normals.push_back(segmentNormal);
    normals.push_back(-segmentNormal);

    segment       = (pointB - pointA);
    segmentLength = segment.getLength();
    segmentNormal = segment.getNormal(halfWidth / segmentLength);

    anchors.push_back(pointA);
    anchors.push_back(pointA);
    normals.push_back(segmentNormal);
    normals.push_back(-segmentNormal);
}

void NoneJoinPolyline::calc_overdraw_vertex_count(bool /*is_looping*/)
{
    overdraw_vertex_count = 4 * (vertex_count - 2); // less than ideal
}

void NoneJoinPolyline::render_overdraw(const std::vector<Vector2>& /*normals*/, float pixel_size,
                                       bool /*is_looping*/)
{
    for (size_t i = 2; i + 3 < vertex_count; i += 4)
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

void NoneJoinPolyline::fill_color_array(Color constant_color, Color* colors, int count)
{
    for (int i = 0; i < count; ++i)
    {
        Color c = constant_color;
        c.a *= (i & 3) < 2; // if (i % 4 == 2 || i % 4 == 3) c.a = 0
        colors[i] = c;
    }
}
