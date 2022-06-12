#pragma once

#include "common/colors.h"
#include "common/vector.h"

#include <memory>
#include <span>
#include <stdint.h>
#include <vector>

namespace love
{
    namespace Vertex
    {
        struct PrimitiveVertex
        {
            float position[3];
            float color[4];
            uint16_t texcoord[2];
        };
        static constexpr int PRIM_VERTEX_SIZE = sizeof(PrimitiveVertex);

        struct GlyphVertex
        {
            float x, y;
            uint16_t s, t;

            Colorf color;
        };
        static constexpr int GLYPH_VERTEX_SIZE = sizeof(GlyphVertex);

        enum CullMode
        {
            CULL_NONE,
            CULL_BACK,
            CULL_FRONT,
            CULL_MAX_ENUM
        };

        enum Winding
        {
            WINDING_CW,
            WINDING_CCW,
            WINDING_MAX_ENUM
        };

        enum TriangleIndexMode
        {
            TRIANGLE_NONE,
            TRIANGLE_STRIP,
            TRIANGLE_FAN,
            TRIANGLE_QUADS
        };

        enum PrimitiveType
        {
            PRIMITIVE_TRIANGLES,
            PRIMITIVE_TRIANGLE_STRIP,
            PRIMITIVE_TRIANGLE_FAN,
            PRIMITIVE_QUADS,
            PRIMITIVE_POINTS,
            PRIMITIVE_MAX_ENUM
        };

        /*
        ** Position, Color
        ** Position, TexCoords, Color
        */
        enum class CommonFormat
        {
            NONE,
            XYf,
            XYZf,
            RGBAub,
            STf_RGBAub,
        };

        static inline CommonFormat Is2DPositionalFormat(bool is2D)
        {
            return is2D ? CommonFormat::XYf : CommonFormat::XYZf;
        }

        static inline uint16_t normto16t(float in)
        {
            return uint16_t(in * 0xFFFF);
        }

        size_t GetFormatStride(CommonFormat format);

        bool GetConstant(const char* in, Winding& out);
        bool GetConstant(Winding in, const char*& out);
        std::vector<const char*> GetConstants(Winding);

        bool GetConstant(const char* in, CullMode& out);
        bool GetConstant(CullMode in, const char*& out);
        std::vector<const char*> GetConstants(CullMode);

        bool GetConstant(const char* in, TriangleIndexMode& out);
        bool GetConstant(TriangleIndexMode in, const char*& out);
        std::vector<const char*> GetConstants(TriangleIndexMode);

        [[nodiscard]] static inline std::unique_ptr<Vertex::PrimitiveVertex[]>
        GeneratePrimitiveFromVectors(std::span<Vector2> points, std::span<Colorf> colors)
        {
            Colorf color = colors[0];

            size_t pointCount = points.size();
            auto result       = std::make_unique<Vertex::PrimitiveVertex[]>(pointCount);

            size_t colorCount = colors.size();

            for (size_t index = 0; index < pointCount; index++)
            {
                const Vector2 point = points[index];

                if (index < colorCount)
                    color = colors[index];

                Vertex::PrimitiveVertex append = { .position = { point.x, point.y, 0.0f },
                                                   .color = { color.r, color.g, color.b, color.a },
                                                   .texcoord = { 0, 0 } };

                result[index] = append;
            }

            return result;
        }

        static inline std::vector<Vertex::PrimitiveVertex> GenerateTextureFromVectors(
            const Vector2* points, const Vector2* texcoord, size_t count, Colorf color)
        {
            std::vector<Vertex::PrimitiveVertex> verts(count);

            for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
            {
                const Vector2 point    = points[currentVertex];
                const Vector2 texCoord = texcoord[currentVertex];

                Vertex::PrimitiveVertex vert = { .position = { point.x, point.y, 0.0f },
                                                 .color    = { 1, 1, 1, 1 },
                                                 .texcoord = { Vertex::normto16t(texCoord.x),
                                                               Vertex::normto16t(texCoord.y) } };

                color.CopyTo(vert.color);

                verts[currentVertex] = vert;
            }

            return verts;
        }

        static inline std::vector<Vertex::PrimitiveVertex> GenerateTextureFromGlyphs(
            const Vertex::GlyphVertex* data, size_t count)
        {
            std::vector<Vertex::PrimitiveVertex> verts(count);

            for (size_t currentVertex = 0; currentVertex < count; currentVertex++)
            {
                const Vertex::GlyphVertex vertex = data[currentVertex];
                Colorf color                     = vertex.color;

                Vertex::PrimitiveVertex vert = { .position = { vertex.x, vertex.y, 0.0f },
                                                 .color    = { 1, 1, 1, 1 },
                                                 .texcoord = { vertex.s, vertex.t } };

                color.CopyTo(vert.color);

                verts[currentVertex] = vert;
            }

            return verts;
        }
    } // namespace Vertex
} // namespace love
