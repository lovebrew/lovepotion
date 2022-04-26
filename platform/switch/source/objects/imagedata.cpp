#include "objects/imagedata/imagedata.h"
#include "modules/thread/types/lock.h"

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

    if (srcformat == dstformat && (sw == dw && dw == sw && sh == dh && dh == sh))
        memcpy(destination, source, srcpixelsize * sw * sh);
    else if (sw > 0)
    {
        // Otherwise, copy each row individually.
        for (int i = 0; i < sh; i++)
        {
            Row rowsrc = { source + (sx + (i + sy) * sw) * srcpixelsize };
            Row rowdst = { destination + (dx + (i + dy) * dw) * dstpixelsize };

            if (srcformat == dstformat)
                memcpy(rowdst.u8, rowsrc.u8, srcpixelsize * sw);
            else
            {
                // Slow path: convert src -> Colorf -> dst.
                Colorf c;
                for (int x = 0; x < sw; x++)
                {
                    auto srcp = (const Pixel*)(rowsrc.u8 + x * srcpixelsize);
                    auto dstp = (Pixel*)(rowdst.u8 + x * dstpixelsize);

                    getFunction(srcp, c);
                    setFunction(c, dstp);
                }
            }
        }
    }
}

static float clamp01(float x)
{
    return std::clamp(x, 0.0f, 1.0f);
}

static void setPixelRGBA8(const Colorf& color, ImageData::Pixel* pixel)
{
    pixel->rgba8[0] = static_cast<uint8_t>(clamp01(color.r) * 0xFF);
    pixel->rgba8[1] = static_cast<uint8_t>(clamp01(color.g) * 0xFF);
    pixel->rgba8[2] = static_cast<uint8_t>(clamp01(color.b) * 0xFF);
    pixel->rgba8[3] = static_cast<uint8_t>(clamp01(color.a) * 0xFF);
}

static void getPixelRGBA8(const ImageData::Pixel* pixel, Colorf& color)
{
    color.r = pixel->rgba8[0] / 0xFF;
    color.g = pixel->rgba8[1] / 0xFF;
    color.b = pixel->rgba8[2] / 0xFF;
    color.a = pixel->rgba8[3] / 0xFF;
}

static void setPixelRGBA16(const Colorf& color, ImageData::Pixel* pixel)
{
    pixel->rgba16[0] = static_cast<uint16_t>(clamp01(color.r) * 0xFFFF + 0.5f);
    pixel->rgba16[1] = static_cast<uint16_t>(clamp01(color.b) * 0xFFFF + 0.5f);
    pixel->rgba16[2] = static_cast<uint16_t>(clamp01(color.g) * 0xFFFF + 0.5f);
    pixel->rgba16[3] = static_cast<uint16_t>(clamp01(color.a) * 0xFFFF + 0.5f);
}

static void getPixelRGBA16(const ImageData::Pixel* pixel, Colorf& color)
{
    color.r = pixel->rgba16[0] / 0xFFFF;
    color.g = pixel->rgba16[1] / 0xFFFF;
    color.b = pixel->rgba16[2] / 0xFFFF;
    color.a = pixel->rgba16[3] / 0xFFFF;
}

ImageData::PixelSetFunction common::ImageData::GetPixelSetFunction(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_RGBA8_UNORM:
            return setPixelRGBA8;
        case PIXELFORMAT_RGBA16_UNORM:
            return setPixelRGBA16;
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
        case PIXELFORMAT_RGBA16_UNORM:
            return getPixelRGBA16;
        default:
            return nullptr;
    }
}
