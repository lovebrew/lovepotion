#pragma once

#include "deko3d/common.h"
#include "deko3d/CMemPool.h"

#include "deko3d/deko.h"

// FreeType2
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class CFont
{
    public:
        constexpr FT_Face & getFace()
        {
            return m_face;
        }

        int loadFace(void * data, size_t size, size_t ptSize);

        bool loadMemory(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                        void * data, size_t size, uint32_t & width, uint32_t & height, DkImageFormat format, uint32_t flags = 0);

    private:
        CImage m_image;
        FT_Face m_face;

        float pointSize;
};