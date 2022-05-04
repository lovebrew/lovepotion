#include "objects/imagedata/imagedatac.h"
#include "modules/filesystem/filesystem.h"

#include "common/bidirectionalmap.h"
#include "common/lmath.h"
#include "modules/image/imagemodule.h"
#include "modules/thread/types/lock.h"

#include <algorithm>

using namespace love::common;
using love::thread::Lock;

love::Type ImageData::type("ImageData", &Data::type);

ImageData::ImageData(Data* data) : ImageDataBase(PIXELFORMAT_UNKNOWN, 0, 0)
{
    this->Decode(data);
}

ImageData::ImageData(int width, int height, PixelFormat format) :
    ImageDataBase(format, width, height)
{
    if (!this->ValidatePixelFormat(format))
        throw love::Exception("ImageData does not support the %s pixel format.",
                              GetPixelFormatName(format));

    this->Create(width, height, format);
    memset(this->data, 0, this->GetSize());
}

ImageData::ImageData(int width, int height, PixelFormat format, void* data, bool own) :
    ImageDataBase(format, width, height)
{
    if (!this->ValidatePixelFormat(format))
        throw love::Exception("ImageData does not support the %s pixel format.",
                              GetPixelFormatName(format));

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

void ImageData::Create(int width, int height, PixelFormat format, void* data)
{
    size_t dataSize = love::GetPixelFormatSliceSize(format, width, height);

    try
    {
        this->data = new uint8_t[dataSize];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory");
    }

    if (data)
        memcpy(this->data, data, dataSize);

    this->decodeHandler = nullptr;
    this->format        = format;

    this->pixelSetFunction = this->GetPixelSetFunction(format);
    this->pixelGetFunction = this->GetPixelGetFunction(format);
}

bool ImageData::ValidatePixelFormat(PixelFormat format)
{
    return love::IsPixelFormatColor(format) && !love::IsPixelFormatCompressed(format);
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
                "Could not decode file '%s' to ImageData: unsupported file format.", name.c_str());
        }
        else
            throw love::Exception(
                "Could not decode data to ImageData: unsupported encoded format.");
    }

    auto expectedSize =
        (decoded.width * decoded.height) * love::GetPixelFormatBlockSize(decoded.format);

    if (decoded.size != expectedSize)
    {
        decoder->FreeRawPixels(decoded.data);
        throw love::Exception("Could not decode image!");
    }

    if (decodeHandler)
        decodeHandler->FreeRawPixels(this->data);
    else
        delete[] this->data;

#if not defined(__3DS__)
    this->width  = decoded.width;
    this->height = decoded.height;
#else
    this->width  = decoded.subWidth;
    this->height = decoded.subHeight;
#endif

    this->data   = decoded.data;
    this->format = decoded.format;

    this->decodeHandler = decoder;

    this->pixelSetFunction = this->GetPixelSetFunction(format);
    this->pixelGetFunction = this->GetPixelGetFunction(format);
}

love::FileData* ImageData::Encode(FormatHandler::EncodedFormat encodedFormat, const char* filename,
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
        GetPixelFormatConstant(format, formatName);
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

bool ImageData::CanPaste(PixelFormat src, PixelFormat dst)
{
    if (src == dst)
        return true;

    if (!(src == PIXELFORMAT_RGBA8_UNORM || src == PIXELFORMAT_RGBA16_UNORM))
        return false;

    if (!(dst == PIXELFORMAT_RGBA8_UNORM || dst == PIXELFORMAT_RGBA16_UNORM))
        return false;

    return true;
}

void ImageData::Paste(ImageData* src, int dx, int dy, int sx, int sy, int sw, int sh)
{
    int srcW = src->GetWidth();
    int srcH = src->GetHeight();
    int dstW = this->GetWidth();
    int dstH = this->GetHeight();

    // Check bounds; if the data ends up completely out of bounds, get out early.
    if (sx >= srcW || sx + sw < 0 || sy >= srcH || sy + sh < 0 || dx >= dstW || dx + sw < 0 ||
        dy >= dstH || dy + sh < 0)
        return;

    // Normalize values to the inside of both images.
    if (dx < 0)
    {
        sw += dx;
        sx -= dx;
        dx = 0;
    }

    if (dy < 0)
    {
        sh += dy;
        sy -= dy;
        dy = 0;
    }

    if (sx < 0)
    {
        sw += sx;
        dx -= sx;
        sx = 0;
    }

    if (sy < 0)
    {
        sh += sy;
        dy -= sy;
        sy = 0;
    }

    if (dx + sw > dstW)
        sw = dstW - dx;

    if (dy + sh > dstH)
        sh = dstH - dy;

    if (sx + sw > srcW)
        sw = srcW - sx;

    if (sy + sh > srcH)
        sh = srcH - sy;

    Lock lock2(src->mutex);
    Lock lock1(this->mutex);

    this->PasteData(src, dx, dy, sx, sy, sw, sh, dstW, dstH);
}

bool ImageData::Inside(int x, int y) const
{
    return x >= 0 && x < this->GetWidth() && y >= 0 && y < this->GetHeight();
}

size_t ImageData::GetSize() const
{
    return love::GetPixelFormatSliceSize(this->format, this->width, this->height);
}

love::thread::Mutex* ImageData::GetMutex() const
{
    return this->mutex;
}

size_t ImageData::GetPixelSize() const
{
    return love::GetPixelFormatBlockSize(this->format);
}

void* ImageData::GetData() const
{
    return this->data;
}

bool ImageData::IsSRGB() const
{
    return false;
}

// clang-format off
constexpr auto encodedFormats = BidirectionalMap<>::Create(
    "png", love::FormatHandler::ENCODED_PNG
);
// clang-format on

bool ImageData::GetConstant(const char* in, FormatHandler::EncodedFormat& out)
{
    return encodedFormats.Find(in, out);
}

bool ImageData::GetConstant(FormatHandler::EncodedFormat in, const char*& out)
{
    return encodedFormats.ReverseFind(in, out);
}

std::vector<const char*> ImageData::GetConstants(FormatHandler::EncodedFormat)
{
    return encodedFormats.GetNames();
}
