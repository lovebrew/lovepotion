#pragma once

#include <common/color.hpp>

#include <utilities/driver/renderer/vertex.hpp>

#include <deko3d.hpp>

#include <array>

namespace love
{
    namespace vertex
    {
        struct Vertex
        {
            float position[3];
            std::array<float, 4> color;
            std::array<uint16_t, 2> texcoord;
        };

        static constexpr size_t VERTEX_SIZE = sizeof(Vertex);

        static std::array<uint16_t, 2> Normalize(const Vector2& in)
        {
            return { normto16t(in.x), normto16t(in.y) };
        }

        namespace attributes
        {
            struct Attribs
            {
                dk::detail::ArrayProxy<const DkVtxAttribState> attributeState;
                dk::detail::ArrayProxy<const DkVtxBufferState> bufferState;
            };

            // clang-format off
            /* Primitives */
            constexpr std::array<DkVtxBufferState, 1> PrimitiveBufferState = {
                DkVtxBufferState { sizeof(Vertex), 0 },
            };

            constexpr std::array<DkVtxAttribState, 2> PrimitiveAttribState = {
                DkVtxAttribState { 0, 0, offsetof(Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
                DkVtxAttribState { 0, 0, offsetof(Vertex, color),    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 }
            };

            /* Textures*/
            constexpr std::array<DkVtxBufferState, 1> TextureBufferState = {
                DkVtxBufferState { sizeof(Vertex), 0 },
            };

            constexpr std::array<DkVtxAttribState, 3> TextureAttribState = {
                DkVtxAttribState { 0, 0, offsetof(Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
                DkVtxAttribState { 0, 0, offsetof(Vertex, color),    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 },
                DkVtxAttribState { 0, 0, offsetof(Vertex, texcoord), DkVtxAttribSize_2x16, DkVtxAttribType_Unorm, 0 }
            };
            // clang-format on

            static inline void GetAttributes(vertex::CommonFormat format, Attribs& out)
            {

                switch (format)
                {
                    case CommonFormat::PRIMITIVE:
                    default:
                    {
                        out.attributeState = PrimitiveAttribState;
                        out.bufferState    = PrimitiveBufferState;
                        break;
                    }
                    case CommonFormat::TEXTURE:
                    {
                        out.attributeState = TextureAttribState;
                        out.bufferState    = TextureBufferState;
                        break;
                    }
                }
            }
        } // namespace attributes
    }     // namespace vertex
} // namespace love
