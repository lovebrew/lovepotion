#pragma once

#include "common/Color.hpp"

#include "modules/graphics/vertex.hpp"

#include <array>
#include <vector>
#define DK_HPP_SUPPORT_VECTOR
#include <deko3d.hpp>

namespace love
{
    namespace vertex
    {
        struct Attributes
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
            DkVtxAttribState { 0, 0, POSITION_OFFSET, DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, COLOR_OFFSET,    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 }
        };

        /* Textures*/
        constexpr std::array<DkVtxBufferState, 1> TextureBufferState = {
            DkVtxBufferState { sizeof(Vertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 3> TextureAttribState = {
            DkVtxAttribState { 0, 0, POSITION_OFFSET, DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, TEXCOORD_OFFSET, DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, COLOR_OFFSET,    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 }
        };
        // clang-format on

        static void getAttributes(bool isTexture, Attributes& out)
        {
            if (!isTexture)
            {
                out.attributeState = PrimitiveAttribState;
                out.bufferState    = PrimitiveBufferState;
                return;
            }

            out.attributeState = TextureAttribState;
            out.bufferState    = TextureBufferState;
        }
    } // namespace vertex
} // namespace love
