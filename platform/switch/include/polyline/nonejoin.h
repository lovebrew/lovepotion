#pragma once

#include "polyline/polyline.h"

namespace love
{
    /*
    ** A Polyline whose segments
    ** are not connected.
    */
    class NoneJoinPolyline : public Polyline
    {
      public:
        NoneJoinPolyline() : Polyline(vertex::TriangleIndexMode::QUADS)
        {
            this->triangleMode = DkPrimitive_Quads;
        }

        void Render(const Vector2* vertices, size_t count, float halfWidth, float pixelSize,
                    bool drawOverdraw)
        {
            Polyline::Render(vertices, count, 4 * count - 4, halfWidth, pixelSize, drawOverdraw);

            // discard the first and last two vertices. (these are redundant)
            for (size_t i = 0; i < this->vertexCount - 4; ++i)
                this->vertices[i] = this->vertices[i + 2];

            // The last quad is now garbage, so zero it out to make sure it doesn't
            // get rasterized. These vertices are in between the core line vertices
            // and the overdraw vertices in the combined vertex array, so they still
            // get "rendered" since we draw everything with one draw call.
            std::fill_n(this->vertices + (this->vertexCount - 4), 4, Vector2 {});

            this->vertexCount -= 4;
        }

      protected:
        void CalculateOverdrawVertexCount(bool /* isLooping */) override;

        void RenderOverdraw(const std::vector<Vector2>& normals, float pixelSize,
                            bool isLooping) override;

        void FillColorArray(Colorf constantColor, Colorf* colors, int count) override;

        void RenderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals, Vector2& s,
                        float& lengthS, Vector2& normalS, const Vector2& q, const Vector2& r,
                        float halfWidth) override;
    };
} // namespace love
