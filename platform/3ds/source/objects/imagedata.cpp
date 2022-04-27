#include "objects/imagedata/imagedata.h"
#include "modules/thread/types/lock.h"

#include "common/lmath.h"

using namespace love;

ImageData::ImageData(Data* data) : common::ImageData(data)
{}

ImageData::ImageData(int width, int height, PixelFormat format) :
    common::ImageData(width, height, format)
{}

ImageData::ImageData(int width, int height, PixelFormat format, void* data, bool own) :
    common::ImageData(width, height, format, data, own)
{}

ImageData::ImageData(const ImageData& other) : common::ImageData(other)
{}

ImageData::~ImageData()
{}

ImageData* ImageData::Clone() const
{
    return new ImageData(*this);
}

void ImageData::GetPixel(int x, int y, Colorf& color) const
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to get out-of-range pixel!");

    size_t pixelSize   = this->GetPixelSize();
    const Pixel* pixel = (const Pixel*)(this->data + ((y * this->width + x) * pixelSize));

    if (this->pixelGetFunction == nullptr)
        throw love::Exception("Unhandled pixel format %d in ImageData::setPixel", format);

    love::thread::Lock lock(this->mutex);
    this->pixelGetFunction(pixel, color);
}

void ImageData::SetPixel(int x, int y, const Colorf& color)
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to set out-of-range pixel!");

    size_t pixelSize = this->GetPixelSize();
    Pixel* pixel     = (Pixel*)(this->data + ((y * this->width + x) * pixelSize));

    if (this->pixelSetFunction == nullptr)
        throw love::Exception("Unhandled pixel format %d in ImageData::setPixel", this->format);

    love::thread::Lock lock(this->mutex);
    this->pixelSetFunction(color, pixel);
}

void ImageData::PasteData(common::ImageData* src, int dx, int dy, int sx, int sy, int sw, int sh,
                          int dw, int dh)
{
    uint8_t* source      = (uint8_t*)src->GetData();
    uint8_t* destination = (uint8_t*)this->GetData();

    size_t srcpixelsize = src->GetPixelSize();
    size_t dstpixelsize = this->GetPixelSize();

    auto getFunction = ((ImageData*)src)->pixelGetFunction;
    auto setFunction = this->pixelSetFunction;

    PixelFormat dstformat = this->GetFormat();
    PixelFormat srcformat = src->GetFormat();

    unsigned _srcPowTwo = NextPo2(((ImageData*)src)->width + 2);
    unsigned _dstPowTwo = NextPo2(this->width + 2);

    for (int y = 0; y < std::min(sh, dh - dy); y++)
    {
        for (int x = 0; x < std::min(sw, dw - dx); x++)
        {
            unsigned srcIndex = coordToIndex(_srcPowTwo, (sx + x) + 1, (sy + y) + 1);
            unsigned dstIndex = coordToIndex(_dstPowTwo, (dx + x) + 1, (dy + y) + 1);

            Colorf color {};

            const Pixel* srcPixel = reinterpret_cast<const Pixel*>((uint32_t*)source + srcIndex);
            getPixelRGBA8(srcPixel, color);

            Pixel* dstPixel = reinterpret_cast<Pixel*>((uint32_t*)this->data + dstIndex);
            setPixelRGBA8(color, dstPixel);
        }
    }
}

static float clamp01(float x)
{
    return std::clamp(x, 0.0f, 1.0f);
}

static void setPixelRGBA8(const Colorf& color, ImageData::Pixel* pixel)
{
    uint8_t r = uint8_t(0xFF * clamp01(color.r) + 0.5f);
    uint8_t g = uint8_t(0xFF * clamp01(color.g) + 0.5f);
    uint8_t b = uint8_t(0xFF * clamp01(color.b) + 0.5f);
    uint8_t a = uint8_t(0xFF * clamp01(color.a) + 0.5f);

    pixel->packed32 = (a | (b << uint32_t(0x08)) | (g << uint32_t(0x10)) | (r << uint32_t(0x18)));
}

static void getPixelRGBA8(const ImageData::Pixel* pixel, Colorf& color)
{
    color.r = ((pixel->packed32 & 0xFF000000) >> 0x18) / 255.0f;
    color.g = ((pixel->packed32 & 0x00FF0000) >> 0x10) / 255.0f;
    color.b = ((pixel->packed32 & 0x0000FF00) >> 0x08) / 255.0f;
    color.a = ((pixel->packed32 & 0x000000FF) >> 0x00) / 255.0f;
}

ImageData::PixelSetFunction common::ImageData::GetPixelSetFunction(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_RGBA8_UNORM:
            return setPixelRGBA8;
        default:
            return nullptr;
    }
}

ImageData::PixelGetFunction common::ImageData::GetPixelGetFunction(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_RGBA8_UNORM:
            return getPixelRGBA8;
        default:
            return nullptr;
    }
}
