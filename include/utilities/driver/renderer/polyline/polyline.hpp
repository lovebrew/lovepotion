#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/vector.hpp>

#include <utilities/driver/renderer/vertex.hpp>

// C++
#include <string.h>
#include <vector>

namespace love
{
    template<Console::Platform T>
    class Graphics;

    /**
     * Abstract base class for a chain of segments.
     * @author Matthias Richter
     **/
    class Polyline
    {
        // treat adjacent segments with angles between their directions <5 degree as straight

      public:
        static constexpr float LINES_PARALLEL_EPS = 0.05f;

        Polyline(vertex::TriangleIndexMode mode = vertex::TRIANGLE_STRIP) :
            vertices(nullptr),
            overdraw(nullptr),
            vertex_count(0),
            overdraw_vertex_count(0),
            triangle_mode(mode),
            overdraw_vertex_start(0)
        {}

        virtual ~Polyline();

        /**
         * @param vertices      Vertices defining the core line segments
         * @param count         Number of vertices
         * @param size_hint     Expected number of vertices of the rendering sleeve around the
         * core line.
         * @param halfwidth     linewidth / 2.
         * @param pixel_size    Dimension of one pixel on the screen in world coordinates.
         * @param draw_overdraw Fake antialias the line.
         */
        void render(const Vector2* vertices, size_t count, size_t size_hint, float halfwidth,
                    float pixel_size, bool draw_overdraw);

        /** Draws the line on the screen
         */
        void draw(Graphics<Console::ALL>* gfx);

      protected:
        virtual void calc_overdraw_vertex_count(bool is_looping);
        virtual void render_overdraw(const std::vector<Vector2>& normals, float pixel_size,
                                     bool is_looping);
        virtual void fill_color_array(Color constant_color, Color* colors, int count);

        /** Calculate line boundary points.
         *
         * @param[out]    anchors       Anchor points defining the core line.
         * @param[out]    normals       Normals defining the edge of the sleeve.
         * @param[in,out] segment       Direction of segment pq (updated to the segment qr).
         * @param[in,out] segmentLength Length of segment pq (updated to the segment qr).
         * @param[in,out] segmentNormal Normal on the segment pq (updated to the segment qr).
         * @param[in]     pointA        Current point on the line (q).
         * @param[in]     pointB        Next point on the line (r).
         * @param[in]     halfWidth     Half line width (see Polyline.render()).
         */
        virtual void renderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                Vector2& segment, float& segmentLength, Vector2& segmentNormal,
                                const Vector2& pointA, const Vector2& pointB, float halfWidth) = 0;

        Vector2* vertices;
        Vector2* overdraw;
        size_t vertex_count;
        size_t overdraw_vertex_count;
        vertex::TriangleIndexMode triangle_mode;
        size_t overdraw_vertex_start;

    }; // Polyline
} // namespace love
