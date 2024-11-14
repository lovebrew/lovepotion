#pragma once

#include <cstring>
#include <gx2/display.h>

namespace love
{
    static void initColorBuffer(GX2ColorBuffer& buffer, int width, int height)
    {
        std::memset(&buffer, 0, sizeof(GX2ColorBuffer));

        buffer.surface.use    = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
        buffer.surface.dim    = GX2_SURFACE_DIM_TEXTURE_2D;
        buffer.surface.aa     = GX2_AA_MODE1X;
        buffer.surface.width  = width;
        buffer.surface.height = height;

        buffer.surface.depth     = 1;
        buffer.surface.mipLevels = 1;

        buffer.surface.format   = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        buffer.surface.tileMode = GX2_TILE_MODE_DEFAULT;
        buffer.viewNumSlices    = 1;

        GX2CalcSurfaceSizeAndAlignment(&buffer.surface);
        GX2InitColorBufferRegs(&buffer);
    }

    static void initDepthBuffer(GX2DepthBuffer& buffer, int width, int height)
    {
        std::memset(&buffer, 0, sizeof(GX2DepthBuffer));

        buffer.surface.use    = GX2_SURFACE_USE_TEXTURE | GX2_SURFACE_USE_DEPTH_BUFFER;
        buffer.surface.dim    = GX2_SURFACE_DIM_TEXTURE_2D;
        buffer.surface.width  = width;
        buffer.surface.height = height;
        buffer.surface.aa     = GX2_AA_MODE1X;

        buffer.surface.depth     = 1;
        buffer.surface.mipLevels = 1;

        buffer.surface.format    = GX2_SURFACE_FORMAT_FLOAT_D24_S8;
        buffer.surface.tileMode  = GX2_TILE_MODE_DEFAULT;
        buffer.surface.mipLevels = 1;

        GX2CalcSurfaceSizeAndAlignment(&buffer.surface);
        GX2InitDepthBufferRegs(&buffer);
    }
} // namespace love
