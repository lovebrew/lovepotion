#include "objects/imagedata/imagedata.h"
#include "modules/filesystem/filesystem.h"

#include "common/lmath.h"
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
    size_t dataSize = 0;

    if (format == PIXELFORMAT_TEX3DS_RGBA8)
        dataSize = NextPO2(width + 2) * NextPO2(height + 2) * GetPixelFormatSize(format);
    else
        dataSize = width * height * GetPixelFormatSize(format);

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
    switch (format)
    {
        case PIXELFORMAT_RGBA8:
        case PIXELFORMAT_RGBA16:
        case PIXELFORMAT_TEX3DS_RGBA8:
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

#if not defined(__3DS__)
    this->width  = decoded.width;
    this->height = decoded.height;
#else
    this->width     = decoded.subWidth;
    this->height    = decoded.subHeight;
#endif

    this->data   = decoded.data;
    this->format = decoded.format;

    this->decodeHandler = decoder;

    this->pixelSetFunction = this->GetPixelSetFunction(format);
    this->pixelGetFunction = this->GetPixelGetFunction(format);
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

static float clamp01(float x)
{
    return std::clamp(x, 0.0f, 1.0f);
}

// static void setPixelRGB(const Colorf& color, ImageData::Pixel* pixel)
// {
//     pixel->rgba8[0] = static_cast<uint8_t>(clamp01(color.r) * 0xFF + 0.5f);
//     pixel->rgba8[1] = static_cast<uint8_t>(clamp01(color.g) * 0xFF + 0.5f);
// }

static void setPixelRGBA8(const Colorf& color, ImageData::Pixel* pixel)
{
    pixel->rgba8[0] = static_cast<uint8_t>(clamp01(color.r) * 0xFF);
    pixel->rgba8[1] = static_cast<uint8_t>(clamp01(color.g) * 0xFF);
    pixel->rgba8[2] = static_cast<uint8_t>(clamp01(color.b) * 0xFF);
    pixel->rgba8[3] = static_cast<uint8_t>(clamp01(color.a) * 0xFF);
}

static void setPixelRGBA16(const Colorf& color, ImageData::Pixel* pixel)
{
    pixel->rgba16[0] = static_cast<uint16_t>(clamp01(color.r) * 0xFFFF + 0.5f);
    pixel->rgba16[1] = static_cast<uint16_t>(clamp01(color.b) * 0xFFFF + 0.5f);
    pixel->rgba16[2] = static_cast<uint16_t>(clamp01(color.g) * 0xFFFF + 0.5f);
    pixel->rgba16[3] = static_cast<uint16_t>(clamp01(color.a) * 0xFFFF + 0.5f);
}

static void setPixelTex3ds(const Colorf& color, ImageData::Pixel* pixel)
{
    uint8_t r = uint8_t(0xFF * clamp01(color.r) + 0.5f);
    uint8_t g = uint8_t(0xFF * clamp01(color.g) + 0.5f);
    uint8_t b = uint8_t(0xFF * clamp01(color.b) + 0.5f);
    uint8_t a = uint8_t(0xFF * clamp01(color.a) + 0.5f);

    pixel->packed32 = (a | (b << uint32_t(0x08)) | (g << uint32_t(0x10)) | (r << uint32_t(0x18)));
}

static void getPixelRGBA8(const ImageData::Pixel* pixel, Colorf& color)
{
    color.r = pixel->rgba8[0] / 0xFF;
    color.g = pixel->rgba8[1] / 0xFF;
    color.b = pixel->rgba8[2] / 0xFF;
    color.a = pixel->rgba8[3] / 0xFF;
}

static void getPixelRGBA16(const ImageData::Pixel* pixel, Colorf& color)
{
    color.r = pixel->rgba16[0] / 0xFFFF;
    color.g = pixel->rgba16[1] / 0xFFFF;
    color.b = pixel->rgba16[2] / 0xFFFF;
    color.a = pixel->rgba16[3] / 0xFFFF;
}

static void getPixelTex3ds(const ImageData::Pixel* pixel, Colorf& color)
{
    color.r = ((pixel->packed32 & 0xFF000000) >> 0x18) / 255.0f;
    color.g = ((pixel->packed32 & 0x00FF0000) >> 0x10) / 255.0f;
    color.b = ((pixel->packed32 & 0x0000FF00) >> 0x08) / 255.0f;
    color.a = ((pixel->packed32 & 0x000000FF) >> 0x00) / 255.0f;
}

void ImageData::SetPixel(int x, int y, const Colorf& color)
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to set out-of-range pixel!");

#if not defined(__3DS__)
    size_t pixelSize = this->GetPixelSize();
    Pixel* pixel     = (Pixel*)(this->data + ((y * this->width + x) * pixelSize));

    if (this->pixelSetFunction == nullptr)
        throw love::Exception("Unhandled pixel format %d in ImageData::setPixel", this->format);

    Lock lock(this->mutex);
    this->pixelSetFunction(color, pixel);
#else
    unsigned _width = NextPO2(this->width + 2);
    unsigned index  = coordToIndex(_width, x + 1, y + 1);

    Pixel* pixel = reinterpret_cast<Pixel*>((uint32_t*)this->data + index);

    Lock lock(this->mutex);
    setPixelTex3ds(color, pixel);
#endif
}

void ImageData::GetPixel(int x, int y, Colorf& color) const
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to get out-of-range pixel!");

#if not defined(__3DS__)
    size_t pixelSize   = this->GetPixelSize();
    const Pixel* pixel = (const Pixel*)(this->data + ((y * this->width + x) * pixelSize));

    if (this->pixelGetFunction == nullptr)
        throw love::Exception("Unhandled pixel format %d in ImageData::setPixel", format);

    Lock lock(this->mutex);
    this->pixelGetFunction(pixel, color);
#else
    unsigned _width = NextPO2(this->width + 2);
    unsigned index  = coordToIndex(_width, x + 1, y + 1);

    const Pixel* pixel = reinterpret_cast<const Pixel*>((uint32_t*)this->data + index);

    Lock lock(this->mutex);
    getPixelTex3ds(pixel, color);
#endif
}

bool ImageData::CanPaste(PixelFormat src, PixelFormat dst)
{
    if (src == dst)
        return true;

    if (!(src == PIXELFORMAT_RGBA8 || src == PIXELFORMAT_RGBA16))
        return false;

    if (!(dst == PIXELFORMAT_RGBA8 || dst == PIXELFORMAT_RGBA16))
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

#if defined(__3DS__)
    unsigned _srcPowTwo = NextPO2(src->width + 2);
    unsigned _dstPowTwo = NextPO2(this->width + 2);

    for (int y = 0; y < std::min(sh, dstH - dy); y++)
    {
        for (int x = 0; x < std::min(sw, dstW - dx); x++)
        {
            unsigned srcIndex = coordToIndex(_srcPowTwo, (sx + x) + 1, (sy + y) + 1);
            unsigned dstIndex = coordToIndex(_dstPowTwo, (dx + x) + 1, (dy + y) + 1);

            Colorf color {};

            const Pixel* srcPixel = reinterpret_cast<const Pixel*>((uint32_t*)src->data + srcIndex);
            getPixelTex3ds(srcPixel, color);

            Pixel* dstPixel = reinterpret_cast<Pixel*>((uint32_t*)this->data + dstIndex);
            setPixelTex3ds(color, dstPixel);
        }
    }
#elif defined(__SWITCH__)
    uint8_t* source      = (uint8_t*)src->GetData();
    uint8_t* destination = (uint8_t*)this->GetData();

    size_t srcpixelsize = src->GetPixelSize();
    size_t dstpixelsize = this->GetPixelSize();

    auto getFunction = src->pixelGetFunction;
    auto setFunction = this->pixelSetFunction;

    PixelFormat dstformat = this->GetFormat();
    PixelFormat srcformat = src->GetFormat();

    if (srcformat == dstformat && (sw == dstW && dstW == srcW && sh == dstH && dstH == srcH))
        memcpy(destination, source, srcpixelsize * sw * sh);
    else if (sw > 0)
    {
        // Otherwise, copy each row individually.
        for (int i = 0; i < sh; i++)
        {
            Row rowsrc = { source + (sx + (i + sy) * srcW) * srcpixelsize };
            Row rowdst = { destination + (dx + (i + dy) * dstW) * dstpixelsize };

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
#endif
}

ImageData::PixelSetFunction ImageData::GetPixelSetFunction(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_TEX3DS_RGBA8:
            return setPixelTex3ds;
        case PIXELFORMAT_RGBA8:
            return setPixelRGBA8;
        case PIXELFORMAT_RGBA16:
            return setPixelRGBA16;
        default:
            return nullptr;
    }
}

ImageData::PixelGetFunction ImageData::GetPixelGetFunction(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_TEX3DS_RGBA8:
            return getPixelTex3ds;
        case PIXELFORMAT_RGBA8:
            return getPixelRGBA8;
        case PIXELFORMAT_RGBA16:
            return getPixelRGBA16;
        default:
            return nullptr;
    }
}

bool ImageData::Inside(int x, int y) const
{
    return x >= 0 && x < this->GetWidth() && y >= 0 && y < this->GetHeight();
}

size_t ImageData::GetSize() const
{
    if (this->format == PIXELFORMAT_TEX3DS_RGBA8)
        return NextPO2(this->width + 2) * NextPO2(this->height + 2) * this->GetPixelSize();
    else
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
