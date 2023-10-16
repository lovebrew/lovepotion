#pragma once

#include <utilities/driver/renderer/polyline/polyline.hpp>

namespace love
{
    /**
     * A Polyline whose segments are connected by a sharp edge.
     * @author Matthias Richter
     */
    class MiterJoinPolyline : public Polyline
    {
      public:
        void render(const Vector2* vertices, size_t count, float halfwidth, float pixel_size,
                    bool draw_overdraw)
        {
            Polyline::render(vertices, count, 2 * count, halfwidth, pixel_size, draw_overdraw);
        }

      protected:
        void renderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals, Vector2& s,
                        float& len_s, Vector2& ns, const Vector2& q, const Vector2& r,
                        float hw) override;

    }; // MiterJoinPolyline
} // namespace love
