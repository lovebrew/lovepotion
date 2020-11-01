#include "common/runtime.h"
#include "deko3d/CFont.h"

#include "deko3d/graphics.h"

/*
** We first need to load the Face (font) via FreeType 2 memory loading.
** Once we do this, we create a big enough "empty" texture to copy the
** glyph data from whatever string we want into the texture itself to be
** rendered
*/
CFont::CFont(void * data, size_t size, int height) : textureWidth(128),
                                                     textureHeight(128)
{
    while (true)
    {
        if ((height * 0.8) * height * 30 <= textureWidth * textureHeight)
            break;

        TextureSize nextsize = this->GetNextTextureSize();

        if (nextsize.width <= textureWidth && nextsize.height <= textureHeight)
            break;

        textureWidth = nextsize.width;
        textureHeight = nextsize.height;
    }

    this->loadFace(data, size, height);
}

int CFont::loadFace(void * data, size_t size, int ptSize)
{
    // Don't allow 0 sized data or nullptr data
    if (size <= 0 || !data)
        return -1;

    /*
    ** load a face from the buffer and given size
    ** faces describe a font
    */
    int error = FT_New_Memory_Face(love::deko3d::Graphics::g_ftLibrary,
                                   (FT_Byte *)data, (FT_Long)size,
                                   0, &m_face);

    if (error == FT_Err_Unknown_File_Format)
        throw love::Exception("Unknown file format");

    error = FT_Set_Pixel_Sizes(m_face, 0, ptSize);

    return error;
}

CFont::TextureSize CFont::GetNextTextureSize() const
{
    TextureSize size = {this->textureWidth, this->textureHeight};

    int maxsize = 2048;
    // auto gfx = Module::getInstance<Graphics>(Module::M_GRAPHICS);
    // if (gfx != nullptr)
    // {
    // 	const auto &caps = gfx->getCapabilities();
    // 	maxsize = (int) caps.limits[Graphics::LIMIT_TEXTURE_SIZE];
    // }

    int maxwidth  = std::min(8192, maxsize);
    int maxheight = std::min(4096, maxsize);

    if (size.width * 2 <= maxwidth || size.height * 2 <= maxheight)
    {
        // {128, 128} -> {256, 128} -> {256, 256} -> {512, 256} -> etc.
        if (size.width == size.height)
            size.width *= 2;
        else
            size.height *= 2;
    }

    return size;
}


bool CFont::loadMemory(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                       void * data, size_t size, uint32_t & width, uint32_t & height, DkImageFormat format, uint32_t flags)
{
    // Don't allow 0 sized data or nullptr data
    if (size <= 0 || !data)
        return false;

    return true;
}