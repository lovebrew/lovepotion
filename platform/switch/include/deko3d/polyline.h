#pragma once

#include "deko3d/vertex.h"
#include "deko3d/deko.h"

#include "common/vector.h"

namespace love
{
    class Graphics;

    class Polyline
    {
        public:
            Polyline(DkPrimitive mode = DkPrimitive_TriangleStrip) : vertices(nullptr),
                                                                     overdraw(nullptr),
                                                                     vertexCount(0),
                                                                     overdrawCount(0),
                                                                     triangleMode(mode),
                                                                     overdrawStart(0)
            {}

            virtual ~Polyline();

            void Draw(Graphics * gfx);

            void Render(const Vector2 * vertices, size_t count, size_t sizeHint, float halfWidth, float pixelSize, bool drawOverdraw);

        protected:
            Vector2 * vertices;
            Vector2 * overdraw;

            size_t vertexCount;
            size_t overdrawCount;
            DkPrimitive triangleMode;
            size_t overdrawStart;

            virtual void CalculateOverdrawVertexCount(bool isLooping);

            virtual void RenderOverdraw(const std::vector<Vector2> & normals, float pixelSize, bool isLooping);

            virtual void FillColorArray(Colorf constantColor, Colorf * colors, int count);

            virtual void RenderEdge(std::vector<Vector2> & anchors, std::vector<Vector2> & normals,
                                    Vector2 & s, float & lengthS, Vector2 & ns, const Vector2 & q,
                                    const Vector2 & r, float hw) = 0;
    };

    class NoneJoinPolyline : public Polyline
    {
        public:
            NoneJoinPolyline() : Polyline(DkPrimitive_Quads)
            {}

            void Render(const Vector2 * vertices, size_t count, float halfwidth, float pixel_size, bool draw_overdraw)
            {
                Polyline::Render(vertices, count, 4 * count - 4, halfwidth, pixel_size, draw_overdraw);

                // discard the first and last two vertices. (these are redundant)
                for (size_t i = 0; i < this->vertexCount - 4; ++i)
                    this->vertices[i] = this->vertices[i + 2];

                // The last quad is now garbage, so zero it out to make sure it doesn't
                // get rasterized. These vertices are in between the core line vertices
                // and the overdraw vertices in the combined vertex array, so they still
                // get "rendered" since we draw everything with one draw call.
                memset(&this->vertices[this->vertexCount - 4], 0, sizeof(love::Vector2) * 4);

                this->vertexCount -= 4;
            }

        protected:
            void CalculateOverdrawVertexCount(bool is_looping) override;
            void RenderOverdraw(const std::vector<Vector2> & normals, float pixel_size, bool is_looping) override;
            void FillColorArray(Colorf constant_color, Colorf * colors, int count) override;
            void RenderEdge(std::vector<Vector2> & anchors, std::vector<Vector2> & normals,
                            Vector2 & s, float & len_s, Vector2 & ns, const Vector2 & q,
                            const Vector2 & r, float hw) override;

    }; // NoneJoinPolyline

    /**
     * A Polyline whose segments are connected by a sharp edge.
     * @author Matthias Richter
     */
    class MiterJoinPolyline : public Polyline
    {
        public:
            void Render(const Vector2 *vertices, size_t count, float halfwidth, float pixel_size, bool draw_overdraw)
            {
                Polyline::Render(vertices, count, 2 * count, halfwidth, pixel_size, draw_overdraw);
            }

        protected:
            void RenderEdge(std::vector<Vector2> &anchors, std::vector<Vector2> &normals,
                            Vector2 &s, float &len_s, Vector2 &ns, const Vector2 &q,
                            const Vector2 &r, float hw) override;

    }; // MiterJoinPolyline

    /**
     * A Polyline whose segments are connected by a flat edge.
     * @author Matthias Richter
     */
    class BevelJoinPolyline : public Polyline
    {
        public:
            void Render(const Vector2 *vertices, size_t count, float halfwidth, float pixel_size, bool draw_overdraw)
            {
                Polyline::Render(vertices, count, 4 * count - 4, halfwidth, pixel_size, draw_overdraw);
            }

        protected:
            void RenderEdge(std::vector<Vector2> &anchors, std::vector<Vector2> &normals,
                            Vector2 &s, float &len_s, Vector2 &ns, const Vector2 &q,
                            const Vector2 &r, float hw) override;

    }; // BevelJoinPolyline
}
