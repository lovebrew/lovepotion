#include "common/runtime.h"
#include "deko3d/CFont.h"

#include "deko3d/graphics.h"

int CFont::loadFace(void * data, size_t size, size_t ptSize)
{
    // Don't allow 0 sized data or nullptr data
    if (size <= 0 || !data)
        return;

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

bool CFont::loadMemory(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                       void * data, size_t size, uint32_t & width, uint32_t & height, DkImageFormat format, uint32_t flags)
{
    // Don't allow 0 sized data or nullptr data
    if (size <= 0 || !data)
        return false;

    return true;
}