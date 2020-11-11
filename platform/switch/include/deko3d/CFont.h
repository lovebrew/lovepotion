#pragma once

#include "deko3d/common.h"
#include "deko3d/CMemPool.h"

#include "objects/texture/texture.h"

#include "truetyperasterizer.h"
#include "common/strongref.h"

#include "deko3d/vertex.h"

// FreeType2
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class CFont
{
    public:
        CFont();

        ~CFont();

        void Load(love::Rasterizer * rasterizer, const love::Texture::Filter & filter);

        struct Glyph
        {
            dk::Image texture;
            CMemPool::Handle memory;

            int spacing;

            int x;
            int y;

            vertex::Vertex vertices[4];
        };

        constexpr dk::ImageDescriptor & getDescriptor()
        {
            return m_descriptor;
        }

        constexpr dk::Image & getImage()
        {
            return m_image;
        }

        void SetHandle(const DkResHandle & handle)
        {
            this->handle = handle;
        }

        std::vector<love::StrongReference<love::Rasterizer>> GetRasterizers()
        {
            return this->rasterizers;
        }

        float GetKerning(uint32_t leftGlyph, uint32_t rightGlyph);

        const Glyph & FindGlyph(uint32_t glyph);

        float GetBaseline() const;

        float GetAscent() const;

        float GetDescent() const;

        float GetHeight() const;

    private:
        void CreateTexture();

        struct TextureSize
        {
            int width;
            int height;
        };

        DkResHandle handle;

        TextureSize GetNextTextureSize() const;

        const Glyph & AddGlyph(uint32_t glyph);

        love::GlyphData * GetRasterizerGlyphData(uint32_t glyph);

        dk::Image m_image;

        CMemPool::Handle m_scratch;
        CMemPool::Handle m_mem;

        dk::ImageDescriptor m_descriptor;

        static const int SPACES_PER_TAB = 4;

        int textureX;
        int textureY;
        int rowHeight;
        int textureCacheID;

        static const int TEXTURE_PADDING = 2;

        float pointSize;
        float height;

        int textureWidth;
        int textureHeight;

        std::vector<love::StrongReference<love::Rasterizer>> rasterizers;

        bool useSpacesAsTab;

        std::vector<dk::Image> images;

        std::unordered_map<uint32_t, Glyph> glyphs;

        std::unordered_map<uint64_t, float> kerning;
};