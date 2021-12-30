#include "objects/imagedata/imagedata.h"
#include "modules/filesystem/filesystem.h"

#include <algorithm>

using namespace love;
using thread::Lock;

love::Type ImageData::type("ImageData", &Data::type);

ImageData::ImageData(Data* data) : ImageDataBase(PIXELFORMAT_UNKNOWN, 0, 0)
{
    this->Decode(data);
}

ImageData::ImageData(int width, int height, PixelFormat format) :
    ImageDataBase(format, width, height)
{
    if (!this->ValidatePixelFormat(format))
        throw love::Exception("Unsupported pixel format for ImageData.");

    this->Create(width, height, format);
    memset(this->data, 0, this->GetSize());
}

ImageData::ImageData(int width, int height, PixelFormat format, void* data, bool own) :
    ImageDataBase(format, width, height)
{
    if (!this->ValidatePixelFormat(format))
        throw love::Exception("Unsupported pixel format for ImageData");

    if (own)
        this->data = (uint8_t*)data;
    else
        this->Create(width, height, format, data);
}

ImageData::ImageData(const ImageData& other) :
    ImageDataBase(other.format, other.width, other.height)
{
    this->Create(width, height, format, other.GetData());
}

ImageData::~ImageData()
{
    if (this->decodeHandler.Get())
        decodeHandler->FreeRawPixels(this->data);
    else
        delete[] this->data;
}

ImageData* ImageData::Clone() const
{
    return new ImageData(*this);
}

void ImageData::Create(int width, int height, PixelFormat format, void* data)
{
    size_t datasize = width * height * GetPixelFormatSize(format);

    try
    {
        this->data = new uint8_t[datasize];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory");
    }

    if (data)
        memcpy(this->data, data, datasize);

    this->decodeHandler = nullptr;
    this->format        = format;
}
