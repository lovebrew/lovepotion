/*
** Sample Framework for deko3d Applications
**   CExternalImage.cpp: Utility class for loading images from the filesystem
*/
#include "deko3d/CImage.h"
#include "deko3d/FileLoader.h"

#include "common/mmath.h"

u32 *CImage::loadPNG(void *buffer, size_t size, int &width, int &height)
{
    if (buffer == nullptr || size <= 0)
        return nullptr;

    u32 *output = nullptr;
    FILE *input = fmemopen(buffer, size, "rb");
    ;

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

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(input);
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

    png_bytep *rowPointers = new (std::nothrow) png_bytep[height];
    if (rowPointers == nullptr)
    {
        png_destroy_read_struct(&png, &info, NULL);
        return nullptr;
    }

    for (int y = 0; y < height; y++)
        rowPointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));

    png_read_image(png, rowPointers);

    fclose(input);
    png_destroy_read_struct(&png, &info, NULL);

    output = new (std::nothrow) u32[width * height];

    if (output == NULL)
    {
        for (int j = 0; j < height; j++)
            free(rowPointers[j]);

        delete[] rowPointers;
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

        /*
        ** free the completed row, to avoid having to
        ** iterate over the whole thing again
        */
        free(rowPointers[j]);
    }

    delete[] rowPointers;

    return output;
}

/*
** Due to the way libjpg-turbo is, this works as-expected, but images must
** have their "progressive" flag turned off, usually dealt with in GIMP or
** similar programs
*/
u8 *CImage::loadJPG(void *buffer, size_t size, int &width, int &height)
{
    u8 *output = nullptr;

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

void * CImage::load(void *buffer, size_t size, int &width, int &height)
{
    if (size <= 0 || !buffer)
        return nullptr;

    void *result = nullptr;

    if (!(result = this->loadPNG(buffer, size, width, height)))
        result = this->loadJPG(buffer, size, width, height);

    return result;
}

bool CImage::fillShadowBuffer(void * data, const love::Rect & rect)
{
    LOG("Filling buffer");
    auto const src = static_cast<uint8_t *>(data);

    for (unsigned y = 0; y < rect.h; ++y)
    {
        if (y + rect.y < 0 || y + rect.y >= height)
            continue;

        for (unsigned x = 0; x < rect.w; ++x)
        {
            if (x + rect.x < 0 || x + rect.x >= width)
                continue;

            shadowBuffer[((y + rect.y) * width + (x + rect.x)) * 4 + 0] = src[(y * rect.w + x) * 4 + 0]; //r
            shadowBuffer[((y + rect.y) * width + (x + rect.x)) * 4 + 1] = src[(y * rect.w + x) * 4 + 1]; //g
            shadowBuffer[((y + rect.y) * width + (x + rect.x)) * 4 + 2] = src[(y * rect.w + x) * 4 + 2]; //b
            shadowBuffer[((y + rect.y) * width + (x + rect.x)) * 4 + 3] = src[(y * rect.w + x) * 4 + 3]; //a
        }
    }
    LOG("Done");
    return true;
}

static FILE *fp = fopen("atlas.pam", "wb");

bool CImage::dumpShadowBuffer()
{
    fputs("P7\n", fp);
    fprintf(fp, "WIDTH %u\n", this->width);
    fprintf(fp, "HEIGHT %u\n", this->height);
    fputs("DEPTH 4\n", fp);
    fputs("MAXVAL 255\n", fp);
    fputs("TUPLTYPE RGB_ALPHA\n", fp);
    fputs("ENDHDR\n", fp);

    auto p = shadowBuffer;
    auto const end = p + (width * height * 4);

    while (p < end)
    {
        auto const rc = fwrite(p, 1, end - p, fp);
        if (rc == 0)
        {
            fclose(fp);
            return false;
        }

        p += rc;
    }

    if (fclose(fp) != 0)
        return false;

    return true;
}

/* replace the pixels at a location */
bool CImage::replacePixels(CMemPool & scratchPool, dk::Device device, void * data, size_t size,
                           dk::Queue transferQueue, const love::Rect & rect)
{
    CMemPool::Handle tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    if (!memcpy(tempImageMemory.getCpuAddr(), data, size))
    {
        tempImageMemory.destroy();
        return false;
    }

    /*
    ** We need to have a command buffer and some more memory for it
    ** so allocate both and add the memory to the temporary command buffer
    */
    dk::UniqueCmdBuf tempCmdBuff = dk::CmdBufMaker{device}.create();
    CMemPool::Handle tempCmdMem = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);
    tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

    dk::ImageView imageView{m_image};

    tempCmdBuff.copyBufferToImage({tempImageMemory.getGpuAddr()}, imageView, {rect.x, rect.y, 0, rect.w, rect.h, 1});

    // Submit the commands to the transfer queue
    transferQueue.submitCommands(tempCmdBuff.finishList());
    transferQueue.waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}

/* load a CImage with transparent black pixels */
bool CImage::loadEmptyPixels(CMemPool &imagePool, CMemPool &scratchPool, dk::Device device, dk::Queue transferQueue,
                             size_t size, uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags)
{
    CMemPool::Handle tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    /* memset for transparent black pixels */
    memset(tempImageMemory.getCpuAddr(), 0, size);

    this->width = width;
    this->height = height;
    LOG("%u %u", this->width, this->height);
    this->shadowBuffer = new uint8_t[size];
    memset(this->shadowBuffer, 0, size);

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

    dk::ImageView imageView{m_image};
    m_descriptor.initialize(m_image);

    tempCmdBuff.copyBufferToImage({tempImageMemory.getGpuAddr()}, imageView, {0, 0, 0, width, height, 1});

    // Submit the commands to the transfer queue
    transferQueue.submitCommands(tempCmdBuff.finishList());
    transferQueue.waitIdle();

    // Destroy the memory we don't need
    tempCmdMem.destroy();
    tempImageMemory.destroy();

    return true;
}

bool CImage::loadMemory(CMemPool &imagePool, CMemPool &scratchPool, dk::Device device, dk::Queue transferQueue,
                        void *data, size_t size, uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags)
{
    // Don't allow 0 sized data or nullptr data
    if (size <= 0 || !data)
        return false;

    // Allocate temporary memory for the image
    CMemPool::Handle tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        return false;

    /*
    ** If we fail to copy to the CPU address, bail out and
    ** destroy the temporary buffer
    */
    if (!memcpy(tempImageMemory.getCpuAddr(), data, size))
    {
        tempImageMemory.destroy();
        return false;
    }

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