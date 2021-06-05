#pragma once

#include "polyline/polyline.h"

namespace love
{
    /*
    ** A Polyline whose segments
    ** are connected by a sharp edge.
    */
    class MiterJoinPolyline : public Polyline
    {
      public:
        void Render(const Vector2* coords, size_t count, float halfWidth, float pixelSize,
                    bool drawOverdraw)
        {
            Polyline::Render(coords, count, 2 * count, halfWidth, pixelSize, drawOverdraw);
        }

      protected:
        void RenderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals, Vector2& s,
                        float& len_s, Vector2& ns, const Vector2& q, const Vector2& r,
                        float hw) override;
    };
} // namespace love
