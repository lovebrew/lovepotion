#pragma once

#if !defined(__3DS__)
    #include <utilities/driver/renderer/polyline/polyline.hpp>

namespace love
{
    /**
     * A Polyline whose segments are connected by a flat edge.
     * @author Matthias Richter
     */
    class BevelJoinPolyline : public Polyline
    {
      public:
        void render(const Vector2* vertices, size_t count, float halfwidth, float pixel_size,
                    bool draw_overdraw)
        {
            Polyline::render(vertices, count, 4 * count - 4, halfwidth, pixel_size, draw_overdraw);
        }

      protected:
        void renderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals, Vector2& s,
                        float& len_s, Vector2& ns, const Vector2& q, const Vector2& r,
                        float hw) override;

    }; // BevelJoinPolyline
} // namespace love
#endif
