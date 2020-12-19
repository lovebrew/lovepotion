/*
** Sample Framework for deko3d Applications
**   CExternalImage.cpp: Utility class for loading images from the filesystem
*/
#include "deko3d/CImage.h"

#include "common/mmath.h"

#include <cstdio>

namespace
{
    void freeRowPointers(png_bytep * rowPointers, unsigned height)
    {
        if (!rowPointers)
            return;

        for (unsigned i = 0; i < height; ++i)
            free(rowPointers[i]);

        delete [] rowPointers;
    }
}

u32 * CImage::loadPNG(void * buffer, size_t size, int & width, int & height)
{
    if (buffer == nullptr || size <= 0)
        return nullptr;

    u32 * output = nullptr;
    FILE * input = fmemopen(buffer, size, "rb");

    constexpr size_t sigLength = 8;
    u8 sig[sigLength] = {0};

    size_t sigSize = fread(sig, sizeof(u8), sigLength, input);
    fseek(input, 0, SEEK_SET);

    if (sigSize < sigLength || png_sig_cmp(sig, 0, sigLength))
    {
        fclose(input);
        return nullptr;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);

    png_bytep * rowPointers = nullptr;

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(input);
        freeRowPointers(rowPointers, height);
        return nullptr;
    }

    png_init_io(png, input);
    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);

    png_byte colorType = png_get_color_type(png, info);
    png_byte bitDepth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, ABGR format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    if (bitDepth == 16)
        png_set_strip_16(png);

    if (colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS) != 0)
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (colorType == PNG_COLOR_TYPE_RGB ||
        colorType == PNG_COLOR_TYPE_GRAY ||
        colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (colorType == PNG_COLOR_TYPE_GRAY ||
        colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    rowPointers = new (std::nothrow) png_bytep[height];

    if (rowPointers == nullptr)
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(input);
        return nullptr;
    }

    auto const rowBytes = png_get_rowbytes(png, info);

    for (int y = 0; y < height; y++)
        rowPointers[y] = (png_byte *)malloc(rowBytes);

    png_read_image(png, rowPointers);

    fclose(input);
    png_destroy_read_struct(&png, &info, NULL);

    output = new (std::nothrow) u32[width * height];

    if (output == NULL)
    {
        freeRowPointers(rowPointers, height);
        return nullptr;
    }

    for (int j = height - 1; j >= 0; j--)
    {
        png_bytep row = rowPointers[j];

        for (int i = 0; i < width; i++)
        {
            png_bytep px = &(row[i * 4]);
            memcpy(&output[(((height - 1) - j) * width) + i], px, sizeof(u32));
        }
    }

    freeRowPointers(rowPointers, height);

    return output;
}

/*
** Due to the way libjpg-turbo is, this works as-expected, but images must
** have their "progressive" flag turned off, usually dealt with in GIMP or
** similar programs
*/
u8 * CImage::loadJPG(void * buffer, size_t size, int & width, int & height)
{
    u8 * output = nullptr;

    tjhandle _jpegDecompressor = tjInitDecompress();

    if (_jpegDecompressor == NULL)
        return nullptr;

    int samp;
    if (tjDecompressHeader2(_jpegDecompressor, (u8 *)buffer, size, &width, &height, &samp) == -1)
    {
        tjDestroy(_jpegDecompressor);
        return nullptr;
    }

    if (tjDecompress2(_jpegDecompressor, (u8 *)buffer, size, output, width, 0, height, TJPF_RGBA, TJFLAG_ACCURATEDCT) == -1)
    {
        tjDestroy(_jpegDecompressor);
        return nullptr;
    }

    tjDestroy(_jpegDecompressor);

    return output;
}

bool CImage::load(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device,
                  dk::Queue transferQueue, void * buffer, size_t size, int & width,
                  int & height)
{
    if (size <= 0 || !buffer)
        return false;

    void * result = nullptr;

    if (!(result = this->loadPNG(buffer, size, width, height)))
        result = this->loadJPG(buffer, size, width, height);

    if (result == nullptr)
        return false;

    bool success = this->loadMemory(imagePool, scratchPool, device, transferQueue,
                                    result, width, height, DkImageFormat_RGBA8_Unorm);

    if (result != nullptr)
        free(result);

    return success;
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
    size_t size = this->getFormatSize(format);
    CMemPool::Handle tempImageMemory = scratchPool.allocate(width * height * size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    /* memset for transparent black pixels */
    std::vector<uint8_t> empty(width * height * size, 0);
    memcpy(tempImageMemory.getCpuAddr(), empty.data(), empty.size());

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
    // Don't allow 0 sized data or nullptr data
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
    tempCmdBuff.copyBufferToImage({tempImageMemory.getGpuAddr()}, imageView, {0, 0, 0, width, height, 1}, DkBlitFlag_FlipY);

    // Submit the commands to the transfer queue
    transferQueue.submitCommands(tempCmdBuff.finishList());
    transferQueue.waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}
