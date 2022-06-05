/*
** Sample Framework for deko3d Applications
**   CExternalImage.cpp: Utility class for loading images from the filesystem
*/
#include "deko3d/CImage.h"
#include "deko3d/deko.h"

#include "common/lmath.h"
#include "common/pixelformat.h"

#include <cstdio>

using namespace love;

bool CImage::load(PixelFormat pixelFormat, bool isSRGB, void* buffer, size_t size, int width,
                  int height, bool empty)
{
    DkImageFormat format;
    if (!::deko3d::GetConstant(pixelFormat, format))
        return false;

    if (!empty)
        return this->loadMemory(buffer, width, height, format);
    else
        return this->loadEmptyPixels(width, height, format);
}

/* replace the pixels at a location */
bool CImage::replacePixels(const void* data, size_t size, const Rect& rect)
{
    if (data == nullptr)
        return false;

    CMemPool::Handle tempImageMemory = ::deko3d::Instance().AllocatePool(
        deko3d::MEMPOOL_DATA, size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    memcpy(tempImageMemory.getCpuAddr(), data, size);

    /*
    ** We need to have a command buffer and some more memory for it
    ** so allocate both and add the memory to the temporary command buffer
    */
    dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker { ::deko3d::Instance().GetDevice() }.create();
    CMemPool::Handle tempCmdMem =
        ::deko3d::Instance().GetMemPool(deko3d::MEMPOOL_DATA).allocate(DK_MEMBLOCK_ALIGNMENT);
    tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

    dk::ImageView imageView { m_image };
    tempCmdBuff.copyBufferToImage(
        { tempImageMemory.getGpuAddr() }, imageView,
        { uint32_t(rect.x), uint32_t(rect.y), 0, uint32_t(rect.w), uint32_t(rect.h), 1 });

    // Submit the commands to the transfer queue
    ::deko3d::Instance().GetTextureQueue().submitCommands(tempCmdBuff.finishList());
    ::deko3d::Instance().GetTextureQueue().waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}

/* load a CImage with transparent black pixels */
bool CImage::loadEmptyPixels(uint32_t width, uint32_t height, DkImageFormat dkFormat,
                             uint32_t flags)
{
    PixelFormat format;
    if (!::deko3d::GetConstant(dkFormat, format))
        return false;

    size_t size = love::GetPixelFormatSliceSize(format, width, height);

    if (size <= 0)
        return false;

    std::vector<uint8_t> empty(size, 0);

    return this->loadMemory(empty.data(), width, height, dkFormat, flags);
}

bool CImage::loadMemory(const void* data, uint32_t width, uint32_t height, DkImageFormat dkFormat,
                        uint32_t flags)
{
    if (data == nullptr)
        return false;

    // Allocate temporary memory for the image
    PixelFormat format;
    if (!::deko3d::GetConstant(dkFormat, format))
        return false;

    size_t size = love::GetPixelFormatSliceSize(format, width, height);

    if (size <= 0)
        return false;

    CMemPool::Handle tempImageMemory = ::deko3d::Instance().AllocatePool(
        deko3d::MEMPOOL_DATA, size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    memcpy(tempImageMemory.getCpuAddr(), data, size);

    /*
    ** We need to have a command buffer and some more memory for it
    ** so allocate both and add the memory to the temporary command buffer
    */
    dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker { ::deko3d::Instance().GetDevice() }.create();
    CMemPool::Handle tempCmdMem =
        ::deko3d::Instance().GetMemPool(deko3d::MEMPOOL_DATA).allocate(DK_MEMBLOCK_ALIGNMENT);

    tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

    // Set the image layout for the image
    dk::ImageLayout layout;
    dk::ImageLayoutMaker { ::deko3d::Instance().GetDevice() }
        .setFlags(flags)
        .setFormat(dkFormat)
        .setDimensions(width, height)
        .initialize(layout);

    // Create the image
    m_mem = ::deko3d::Instance().AllocatePool(deko3d::MEMPOOL_IMAGES, layout.getSize(),
                                              layout.getAlignment());

    m_image.initialize(layout, m_mem.getMemBlock(), m_mem.getOffset());
    m_descriptor.initialize(m_image);

    /*
    ** Create the image's view and copy the data
    ** to the temporary image memory
    */
    dk::ImageView imageView { m_image };
    tempCmdBuff.copyBufferToImage({ tempImageMemory.getGpuAddr() }, imageView,
                                  { 0, 0, 0, width, height, 1 }, 0);

    // Submit the commands to the transfer queue
    ::deko3d::Instance().GetTextureQueue().submitCommands(tempCmdBuff.finishList());
    ::deko3d::Instance().GetTextureQueue().waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}
