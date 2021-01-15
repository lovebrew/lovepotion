/*
** Sample Framework for deko3d Applications
**   CExternalImage.cpp: Utility class for loading images from the filesystem
*/
#include "deko3d/CImage.h"

#include "common/lmath.h"

#include <cstdio>

#include <libpng16/png.h>
#include <turbojpeg.h>

#include "common/debug/logger.h"

/*
** Load the specified @buffer with @size into an std::unique_ptr
*/
[[ nodiscard ]] std::unique_ptr<u32[]> CImage::loadPNG(const void * buffer, const size_t size, int & width, int & height)
{
    png_image image;
    memset(&image, 0, sizeof(image));

    image.version = PNG_IMAGE_VERSION;

    png_image_begin_read_from_memory(&image, buffer, size);

    if (PNG_IMAGE_FAILED(image))
    {
        png_image_free(&image);
        return nullptr;
    }

    image.format = PNG_FORMAT_RGBA;

    width  = image.width;
    height = image.height;

    std::unique_ptr<u32[]> outBuffer = std::make_unique<u32[]>(width * height);

    png_image_finish_read(&image, NULL, outBuffer.get(), PNG_IMAGE_ROW_STRIDE(image), NULL);
    png_image_free(&image);

    if (PNG_IMAGE_FAILED(image))
        return nullptr;

    return outBuffer;
}

/*
** Due to the way libjpg-turbo is, this works as-expected, but images must
** have their "progressive" flag turned off, usually dealt with in GIMP or
** similar programs
*/
[[ nodiscard ]] std::unique_ptr<u8[]> CImage::loadJPG(const void * buffer, size_t size, int & width, int & height)
{

    tjhandle _jpegDecompressor = tjInitDecompress();

    if (_jpegDecompressor == NULL)
        return nullptr;

    int samples;
    if (tjDecompressHeader2(_jpegDecompressor, (u8 *)buffer, size, &width, &height, &samples) == -1)
        return nullptr;

    std::unique_ptr<u8[]> outBuffer = std::make_unique<u8[]>(width * height * 4);

    /* we always want RGBA, hopefully outputs as RGBA8 */
    if (tjDecompress2(_jpegDecompressor, (u8 *)buffer, size, outBuffer.get(), width, 0, height, TJPF_RGBA, TJFLAG_ACCURATEDCT) == -1)
        goto fail0;

    tjDestroy(_jpegDecompressor);

    return outBuffer;

    fail0:
        tjDestroy(_jpegDecompressor);
        return nullptr;
}

bool CImage::load(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device,
                  dk::Queue transferQueue, void * buffer, size_t size, int & width,
                  int & height)
{
    DkImageFormat imageFormat = DkImageFormat_RGBA8_Unorm;

    if (auto uniqueBuffer = this->loadPNG(buffer, size, width, height); uniqueBuffer)
        return this->loadMemory(imagePool, scratchPool, device, transferQueue, uniqueBuffer.get(), width, height, imageFormat);
    else if (auto uniqueBuffer = this->loadJPG(buffer, size, width, height); uniqueBuffer)
        return this->loadMemory(imagePool, scratchPool, device, transferQueue, uniqueBuffer.get(), width, height, imageFormat);

    return false;
}

size_t CImage::getFormatSize(DkImageFormat format)
{
    switch (format)
    {
        case DkImageFormat_RGBA8_Unorm:
            return 4;
        default:
            break;
    }

    /* shouldn't happen */

    return 0;
}

/* replace the pixels at a location */
bool CImage::replacePixels(CMemPool & scratchPool, dk::Device device, const void * data, size_t size,
                           dk::Queue transferQueue, const love::Rect & rect)
{
    if (data == nullptr)
        return false;

    CMemPool::Handle tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    memcpy(tempImageMemory.getCpuAddr(), data, size);

    /*
    ** We need to have a command buffer and some more memory for it
    ** so allocate both and add the memory to the temporary command buffer
    */
    dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker{device}.create();
    CMemPool::Handle tempCmdMem = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);
    tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

    dk::ImageView imageView{m_image};
    tempCmdBuff.copyBufferToImage({tempImageMemory.getGpuAddr()}, imageView, {uint32_t(rect.x), uint32_t(rect.y), 0, uint32_t(rect.w), uint32_t(rect.h), 1});

    // Submit the commands to the transfer queue
    transferQueue.submitCommands(tempCmdBuff.finishList());
    transferQueue.waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}

/* load a CImage with transparent black pixels */
bool CImage::loadEmptyPixels(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                             uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags)
{
    size_t size = width * height * this->getFormatSize(format);

    if (size <= 0)
        return false;

    CMemPool::Handle tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    /* memcpy for transparent black pixels */
    std::vector<uint8_t> empty(size, 0);
    memcpy(tempImageMemory.getCpuAddr(), empty.data(), sizeof(uint8_t) * empty.size());

    /*
    ** We need to have a command buffer and some more memory for it
    ** so allocate both and add the memory to the temporary command buffer
    */
    dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker{device}.create();
    CMemPool::Handle tempCmdMem = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);
    tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

    // Set the image layout for the image
    dk::ImageLayout layout;
    dk::ImageLayoutMaker{device}
        .setFlags(flags)
        .setFormat(format)
        .setDimensions(width, height)
        .initialize(layout);

    // Create the image
    m_mem = imagePool.allocate(layout.getSize(), layout.getAlignment());
    m_image.initialize(layout, m_mem.getMemBlock(), m_mem.getOffset());
    m_descriptor.initialize(m_image);

    dk::ImageView imageView{m_image};
    tempCmdBuff.copyBufferToImage({tempImageMemory.getGpuAddr()}, imageView, {0, 0, 0, width, height, 1});

    // Submit the commands to the transfer queue
    transferQueue.submitCommands(tempCmdBuff.finishList());
    transferQueue.waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}

bool CImage::loadMemory(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                        const void * data, uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags)
{
    if (data == nullptr)
        return false;

    // Allocate temporary memory for the image
    size_t size = width * height * this->getFormatSize(format);

    if (size <= 0)
        return false;

    CMemPool::Handle tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    memcpy(tempImageMemory.getCpuAddr(), data, size);

    /*
    ** We need to have a command buffer and some more memory for it
    ** so allocate both and add the memory to the temporary command buffer
    */
    dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker{device}.create();
    CMemPool::Handle tempCmdMem = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);
    tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

    // Set the image layout for the image
    dk::ImageLayout layout;
    dk::ImageLayoutMaker{device}
        .setFlags(flags)
        .setFormat(format)
        .setDimensions(width, height)
        .initialize(layout);

    // Create the image
    m_mem = imagePool.allocate(layout.getSize(), layout.getAlignment());
    m_image.initialize(layout, m_mem.getMemBlock(), m_mem.getOffset());
    m_descriptor.initialize(m_image);

    /*
    ** Create the image's view and copy the data
    ** to the temporary image memory
    */
    dk::ImageView imageView{m_image};
    tempCmdBuff.copyBufferToImage({tempImageMemory.getGpuAddr()}, imageView, {0, 0, 0, width, height, 1}, 0);

    // Submit the commands to the transfer queue
    transferQueue.submitCommands(tempCmdBuff.finishList());
    transferQueue.waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}
