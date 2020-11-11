#include "common/runtime.h"
#include "deko3d/CFont.h"

#include "deko3d/deko.h"

#include "deko3d/graphics.h"
#include "objects/font/font.h"

#include "modules/font/fontmodule.h"
#include "freetype/glyphdata.h"

#include "common/module.h"
#include "modules/graphics/graphics.h"

#define FONT_TEXTURE_SIZE 0x200

CFont::CFont() : textureWidth(128),
                 textureHeight(128),
                 useSpacesAsTab(false),
                 textureCacheID(0)
{}

/*
** We first need to load the Face (font) via FreeType 2 memory loading.
** Once we do this, we create a big enough "empty" texture to copy the
** glyph data from whatever string we want into the texture itself to be
** rendered
*/
void CFont::Load(love::Rasterizer * rasterizer, const love::Texture::Filter & filter)
{
    this->rasterizers = {rasterizer};

    this->glyphs.clear();

    while (true)
    {
        if ((rasterizer->GetHeight() * 0.8) * rasterizer->GetHeight() * 30 <= textureWidth * textureHeight)
            break;

        TextureSize nextsize = this->GetNextTextureSize();

        if (nextsize.width <= textureWidth && nextsize.height <= textureHeight)
            break;

        textureWidth = nextsize.width;
        textureHeight = nextsize.height;
    }

    if (!rasterizer->HasGlyph(9)) // No tab character in the Rasterizer.
        this->useSpacesAsTab = true;

    this->rowHeight = this->textureX = this->textureY = this->TEXTURE_PADDING;
}

CFont::~CFont()
{
    this->glyphs.clear();
    m_scratch.destroy();
}

static inline uint16_t normToUint16(double n)
{
    return (uint16_t)(n * std::numeric_limits<uint16_t>::max());
}

void CFont::CreateTexture()
{
    TextureSize size = {this->textureWidth, this->textureHeight};
    TextureSize nextsize = this->GetNextTextureSize();
    bool recreatetexture = false;

    // If we have an existing texture already, we'll try replacing it with a
    // larger-sized one rather than creating a second one. Having a single
    // texture reduces texture switches and draw calls when rendering.
    if ((nextsize.width > size.width || nextsize.height > size.height) && !images.empty())
    {
        recreatetexture = true;
        size = nextsize;
        images.pop_back();
    }

    // Create the image scratch pool for use
    m_scratch = dk3d.GetData()->allocate(size.width * size.height * 4, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    dk::ImageLayout layout;
    dk::ImageLayoutMaker{dk3d.GetDevice()}
        .setFlags(0)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(size.width, size.height)
        .initialize(layout);

    // allocate the memory for the layout, keep it around
    m_mem = dk3d.GetImages()->allocate(layout.getSize(), layout.getAlignment());
    m_image.initialize(layout, m_mem.getMemBlock(), m_mem.getOffset());

    {
        size_t pixelcount = size.width * size.height;

        // Initialize the texture with transparent white for Luminance-Alpha
        // formats (since we keep luminance constant and vary alpha in those
        // glyphs), and transparent black otherwise.
        std::vector<uint8_t> emptydata(pixelcount * 4, 0);

        Rect rect = {0, 0, size.width, size.height};

        // Replace data
        dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker{dk3d.GetDevice()}.create();
        CMemPool::Handle tempCmdMem = dk3d.GetData()->allocate(DK_MEMBLOCK_ALIGNMENT);
        tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

        dk::ImageView imageView {m_image};

        // Copy the empty data to the Glyph image
        memcpy(m_scratch.getCpuAddr(), emptydata.data(), emptydata.size());
        tempCmdBuff.copyBufferToImage({ m_scratch.getGpuAddr() }, imageView, { 0, 0, size.width, size.height, 1 }, DkBlitFlag_FlipY);

        dk3d.GetTextureQueue().submitCommands(tempCmdBuff.finishList());
        dk3d.GetTextureQueue().waitIdle();

        tempCmdMem.destroy();
    }

    if (recreatetexture)
    {
        textureCacheID++;

        std::vector<uint32_t> glyphsToAdd;

        for (const auto & glyphPair : glyphs)
            glyphsToAdd.push_back(glyphPair.first);

        glyphs.clear();

        for (uint32_t glyph : glyphsToAdd)
            this->AddGlyph(glyph);
    }
}

const CFont::Glyph & CFont::AddGlyph(uint32_t glyph)
{
    love::StrongReference<love::GlyphData> gd(this->GetRasterizerGlyphData(glyph), Acquire::NORETAIN);

    int width  = gd->GetWidth();
    int height = gd->GetHeight();

    if (width + this->TEXTURE_PADDING * 2 < this->textureWidth && height + this->TEXTURE_PADDING * 2 < this->textureHeight)
    {
        if (this->textureX + width + this->TEXTURE_PADDING > this->textureWidth)
        {
            // Out of space - new row!
            this->textureX = this->TEXTURE_PADDING;
            this->textureY += this->rowHeight;
            this->rowHeight = this->TEXTURE_PADDING;
        }

        if (this->textureY + height + this->TEXTURE_PADDING > this->textureHeight)
        {
            // Totally out of space - new texture!
            this->CreateTexture();

            // Makes sure the above code for checking if the glyph can fit at
            // the current position in the texture is run again for this glyph.
            return this->AddGlyph(glyph);
        }
    }

    Glyph g;

    g.spacing = floorf(gd->GetAdvance() / 1.0f + 0.5f);
    memset(g.vertices, 0, sizeof(vertex::Vertex) * 4);

    // Don't waste space on empty glyphs
    if (width > 0 && height > 0)
    {
        // Create a temporary command buffer and memory for copy use
        dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker{dk3d.GetDevice()}.create();
        CMemPool::Handle tempCmdMem = dk3d.GetData()->allocate(DK_MEMBLOCK_ALIGNMENT);
        tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

        // Destination rectangle for the glyph
        Rect rect = {this->textureX, this->textureY, width, height};
        dk::ImageView imageView{m_image};

        // Copy the glyph data to the font's dk::Image
        memcpy(m_scratch.getCpuAddr(), gd->GetData(), gd->GetSize());
        tempCmdBuff.copyBufferToImage({ m_scratch.getGpuAddr() }, imageView, { rect.x, rect.y, 0, width, height, 1 }, DkBlitFlag_FlipY);

        dk3d.GetTextureQueue().submitCommands(tempCmdBuff.finishList());
        dk3d.GetTextureQueue().waitIdle();

        tempCmdMem.destroy();
    }

    this->glyphs.emplace(glyph, g);

    return this->glyphs[glyph];
}

love::GlyphData * CFont::GetRasterizerGlyphData(uint32_t glyph)
{
    // Use spaces for the tab 'glyph'.
    if (glyph == 9 && useSpacesAsTab)
    {
        love::GlyphData * spacegd = rasterizers[0]->GetGlyphData(32);

        love::GlyphData::GlyphMetrics gm = {};

        gm.advance  = spacegd->GetAdvance() * SPACES_PER_TAB;
        gm.bearingX = spacegd->GetBearingX();
        gm.bearingY = spacegd->GetBearingY();

        spacegd->Release();

        return new love::GlyphData(glyph, gm);
    }

    for (const love::StrongReference<love::Rasterizer> & r : rasterizers)
    {
        if (r->HasGlyph(glyph))
            return r->GetGlyphData(glyph);
    }

    return rasterizers[0]->GetGlyphData(glyph);
}

float CFont::GetKerning(uint32_t leftGlyph, uint32_t rightGlyph)
{
    uint64_t packed = ((uint64_t)leftGlyph << 32) | (uint64_t)rightGlyph;
    const auto iterator = this->kerning.find(packed);

    if (iterator != this->kerning.end())
        return iterator->second;

    float kern = rasterizers[0]->GetKerning(leftGlyph, rightGlyph);

    for (const auto & rasterizer : rasterizers)
    {
        if (rasterizer->HasGlyph(leftGlyph) && rasterizer->HasGlyph(rightGlyph))
        {
            kern = floorf(rasterizer->GetKerning(leftGlyph, rightGlyph) / 1.0f + 0.5f);
            break;
        }
    }

    this->kerning[packed] = kern;
    return kern;
}


float CFont::GetAscent() const
{
    return floorf(this->rasterizers[0]->GetAscent() / 1.0f + 0.5f);
}

float CFont::GetDescent() const
{
    return floorf(this->rasterizers[0]->GetDescent() / 1.0f + 0.5f);
}

float CFont::GetHeight() const
{
    return floorf(this->height / 1.0f + 0.5f);
}

float CFont::GetBaseline() const
{
    float ascent = this->GetAscent();

    if (ascent != 0.0f)
        return ascent;
    else if (this->rasterizers[0]->GetDataType() == love::Rasterizer::DATA_TRUETYPE)
        return floorf(this->GetHeight() / 1.25f + 0.5f);
    else
        return 0.0f;
}

const CFont::Glyph & CFont::FindGlyph(uint32_t glyph)
{
    const auto it = this->glyphs.find(glyph);

    if (it != this->glyphs.end())
        return it->second;
    LOG("Returning the returned glyph");
    return this->AddGlyph(glyph);
}

CFont::TextureSize CFont::GetNextTextureSize() const
{
    TextureSize size = {this->textureWidth, this->textureHeight};

    int maxSize = 2048;

    int maxWidth  = std::min(8192, maxSize);
    int maxHeight = std::min(4096, maxSize);

    if (size.width * 2 <= maxWidth || size.height * 2 <= maxHeight)
    {
        // {128, 128} -> {256, 128} -> {256, 256} -> {512, 256} -> etc.
        if (size.width == size.height)
            size.width *= 2;
        else
            size.height *= 2;
    }

    return size;
}