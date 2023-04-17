#pragma once

#if !defined(__3DS__)
    #include <utilities/driver/renderer/polyline/polyline.hpp>

namespace love
{
    /**
     * A Polyline whose segments are not connected.
     * @author Matthias Richter
     */
    class NoneJoinPolyline : public Polyline
    {
      public:
        NoneJoinPolyline() : Polyline(vertex::TRIANGLE_QUADS)
        {}

        void render(const Vector2* vertices, size_t count, float halfwidth, float pixel_size,
                    bool draw_overdraw)
        {
            Polyline::render(vertices, count, 4 * count - 4, halfwidth, pixel_size, draw_overdraw);

            // discard the first and last two vertices. (these are redundant)
            for (size_t i = 0; i < vertex_count - 4; ++i)
                this->vertices[i] = this->vertices[i + 2];

            // The last quad is now garbage, so zero it out to make sure it doesn't
            // get rasterized. These vertices are in between the core line vertices
            // and the overdraw vertices in the combined vertex array, so they still
            // get "rendered" since we draw everything with one draw call.
            memset(&this->vertices[vertex_count - 4], 0, sizeof(love::Vector2) * 4);

            vertex_count -= 4;
        }

      protected:
        void calc_overdraw_vertex_count(bool is_looping) override;
        void render_overdraw(const std::vector<Vector2>& normals, float pixel_size,
                             bool is_looping) override;
        void fill_color_array(Color constant_color, Color* colors, int count) override;
        void renderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals, Vector2& s,
                        float& len_s, Vector2& ns, const Vector2& q, const Vector2& r,
                        float hw) override;

    }; // NoneJoinPolyline
} // namespace love
#endif
