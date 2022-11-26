#include <objects/imagedata_ext.hpp>

using namespace love;

ImageData<Console::CTR>::~ImageData()
{}

ImageData<Console::CTR>::ImageData(Data* data) : ImageData<>(PIXELFORMAT_UNKNOWN, 0, 0)
{
    this->Decode(data);
}

static void setPixelRGBA8(const Color& color, ImageData<Console::CTR>::Pixel* pixel)
{
    pixel->packed32 = color.abgr();
}

static void getPixelRGBA8(const ImageData<Console::CTR>::Pixel* pixel, Color& color)
{
    color = Color(pixel->packed32);
}

void ImageData<Console::CTR>::Create(int width, int height, PixelFormat format, void* data)
{
    const auto size = GetPixelFormatSliceSize(format, width, height);

    try
    {
        this->data = std::make_unique<uint8_t[]>(size);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    if (data)
        std::copy_n(data, this->data.get(), size);

    this->decoder = nullptr;

    this->pixelSetFunction = setPixelRGBA8;
    this->pixelGetFunction = getPixelRGBA8;
}
