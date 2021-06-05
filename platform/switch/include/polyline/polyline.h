#pragma once

#include "deko3d/vertex.h"
#include <span>

namespace love
{
    class Graphics;

    /*
    ** Abstract base class
    ** for a chain of segments.
    */
    class Polyline
    {
      public:
        Polyline(vertex::TriangleIndexMode mode = vertex::TriangleIndexMode::STRIP) :
            vertices(nullptr),
            overdraw(nullptr),
            vertexCount(0),
            overdrawVertexCount(0),
            triangleIndexMode(mode),
            overdrawVertexStart(0)
        {}

        virtual ~Polyline();

        /**
         * @param coords      Vertices defining the core line segments
         * @param count         Number of vertices
         * @param size_hint     Expected number of vertices of the rendering sleeve around the core
         * line.
         * @param halfwidth     linewidth / 2.
         * @param pixel_size    Dimension of one pixel on the screen in world coordinates.
         * @param draw_overdraw Fake antialias the line.
         */
        void Render(const Vector2* coords, size_t count, size_t sizeHint, float halfWidth,
                    float pixelSize, bool drawOverdraw);

        void Draw(Graphics* graphics);

      protected:
        virtual void CalculateOverdrawVertexCount(bool isLooping);

        virtual void RenderOverdraw(const std::vector<Vector2>& normals, float pixelSize,
                                    bool isLooping);

        /* Enables a "fake" anti-aliasing line border */
        virtual void FillColorArray(Colorf constant, Colorf* colors, int count);

        /** Calculate line boundary points.
         *
         * @param[out]    anchors Anchor points defining the core line.
         * @param[out]    normals Normals defining the edge of the sleeve.
         * @param[in,out] s       Direction of segment pq (updated to the segment qr).
         * @param[in,out] len_s   Length of segment pq (updated to the segment qr).
         * @param[in,out] ns      Normal on the segment pq (updated to the segment qr).
         * @param[in]     q       Current point on the line.
         * @param[in]     r       Next point on the line.
         * @param[in]     hw      Half line width (see Polyline.render()).
         */
        virtual void RenderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                Vector2& s, float& len_s, Vector2& ns, const Vector2& q,
                                const Vector2& r, float hw) = 0;

        static constexpr float LINES_PARALLEL_EPS = 0.05f;

        Vector2* vertices;
        Vector2* overdraw;

        size_t vertexCount;
        size_t overdrawVertexCount;

        vertex::TriangleIndexMode triangleIndexMode;
        size_t overdrawVertexStart;

        DkPrimitive triangleMode = DkPrimitive_TriangleStrip;
    };
} // namespace love
