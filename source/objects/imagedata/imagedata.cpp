#include "objects/imagedata/imagedata.h"
#include "modules/filesystem/filesystem.h"

#include "modules/image/imagemodule.h"

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

bool ImageData::ValidatePixelFormat(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_RGBA8:
        case PIXELFORMAT_RGBA16:
            return true;
        default:
            return false;
    }
}

void ImageData::Decode(Data* data)
{
    FormatHandler* decoder = nullptr;
    FormatHandler::DecodedImage decoded {};

    auto module = Module::GetInstance<ImageModule>(Module::M_IMAGE);
    if (module == nullptr)
        throw love::Exception("love.image must be loaded in order to decode ImageData.");

    for (FormatHandler* handler : module->GetFormatHandlers())
    {
        if (handler->CanDecode(data))
        {
            decoder = handler;
            break;
        }
    }

    if (decoder)
        decoded = decoder->Decode(data);

    if (decoded.data == nullptr)
    {
        auto fileData = static_cast<FileData*>(data);

        if (fileData != nullptr)
        {
            const std::string& name = fileData->GetFilename();
            throw love::Exception(
                "Could not decode file '%s' to ImageData: unsupported file format", name.c_str());
        }
        else
            throw love::Exception("Could not decode data to ImageData: unsupported encoded format");
    }

    if (decoded.size != (decoded.width * decoded.height) * GetPixelFormatSize(decoded.format))
    {
        decoder->FreeRawPixels(decoded.data);
        throw love::Exception("Could not decode image!");
    }

    if (decodeHandler)
        decodeHandler->FreeRawPixels(this->data);
    else
        delete[] this->data;

    this->width  = decoded.width;
    this->height = decoded.height;
    this->data   = decoded.data;
    this->format = decoded.format;

    this->decodeHandler = decoder;
}

FileData* ImageData::Encode(FormatHandler::EncodedFormat encodedFormat, const char* filename,
                            bool writefile) const
{
    FormatHandler* encoder = nullptr;

    FormatHandler::EncodedImage encoded {};
    FormatHandler::DecodedImage decoded {};

    decoded.width  = width;
    decoded.height = height;
    decoded.size   = this->GetSize();
    decoded.data   = data;
    decoded.format = format;

    auto module = Module::GetInstance<ImageModule>(Module::M_IMAGE);

    if (module == nullptr)
        throw love::Exception("love.image must be loaded in order to encode ImageData.");

    for (FormatHandler* handler : module->GetFormatHandlers())
    {
        if (handler->CanEncode(format, encodedFormat))
        {
            encoder = handler;
            break;
        }
    }

    if (encoder != nullptr)
    {
        thread::Lock lock(this->mutex);
        encoded = encoder->Encode(decoded, encodedFormat);
    }

    if (encoder == nullptr || encoded.data == nullptr)
    {
        const char* formatName = "unknown";
        ImageModule::GetConstant(format, formatName);
        throw love::Exception("No suitable image encoder for '%s' format.", formatName);
    }

    FileData* fileData = nullptr;

    try
    {
        fileData = new FileData(encoded.size, filename);
    }
    catch (love::Exception&)
    {
        encoder->FreeRawPixels(encoded.data);
        throw;
    }

    memcpy(fileData->GetData(), encoded.data, encoded.size);
    encoder->FreeRawPixels(encoded.data);

    if (writefile)
    {
        auto filesystem = Module::GetInstance<Filesystem>(Module::M_FILESYSTEM);
        if (filesystem == nullptr)
        {
            fileData->Release();
            throw love::Exception(
                "love.filesystem must be loaded in order to write encoded ImageData.");
        }

        try
        {
            filesystem->Write(filename, fileData->GetData(), fileData->GetSize());
        }
        catch (love::Exception&)
        {
            fileData->Release();
            throw;
        }
    }

    return fileData;
}

void ImageData::SetPixel(int x, int y, const Colorf& color)
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to set out-of-range pixel!");
}

void ImageData::GetPixel(int x, int y, Colorf& color) const
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to get out-of-range pixel!");
}

void ImageData::Paste(ImageData* src, int dx, int dy, int sx, int sy, int sw, int sh)
{
    PixelFormat dstformat = this->GetFormat();
    PixelFormat srcformat = src->GetFormat();

    int srcW = src->GetWidth();
    int srcH = src->GetHeight();
    int dstW = this->GetWidth();
    int dstH = this->GetHeight();

    size_t srcpixelsize = src->GetPixelSize();
    size_t dstpixelsize = this->GetPixelSize();
}

bool ImageData::Inside(int x, int y) const
{
    return x >= 0 && x < this->GetWidth() && y >= 0 && y < this->GetHeight();
}

size_t ImageData::GetSize() const
{
    return size_t(this->GetWidth() * this->GetHeight()) * this->GetPixelSize();
}

thread::Mutex* ImageData::GetMutex() const
{
    return this->mutex;
}

size_t ImageData::GetPixelSize() const
{
    return love::GetPixelFormatSize(format);
}

void* ImageData::GetData() const
{
    return this->data;
}

bool ImageData::IsSRGB() const
{
    return false;
}

bool ImageData::GetConstant(const char* in, FormatHandler::EncodedFormat& out)
{
    return encodedFormats.Find(in, out);
}

bool ImageData::GetConstant(FormatHandler::EncodedFormat in, const char*& out)
{
    return encodedFormats.Find(in, out);
}

std::vector<const char*> ImageData::GetConstants(FormatHandler::EncodedFormat)
{
    return encodedFormats.GetNames();
}

// clang-format off
constexpr StringMap<FormatHandler::EncodedFormat, FormatHandler::ENCODED_MAX_ENUM>::Entry formatEntries[] =
{
    { "png", FormatHandler::ENCODED_PNG }
};

constinit const StringMap<FormatHandler::EncodedFormat, FormatHandler::ENCODED_MAX_ENUM> ImageData::encodedFormats(formatEntries);
// clang-format on
