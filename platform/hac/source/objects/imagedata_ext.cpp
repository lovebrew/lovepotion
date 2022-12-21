#include <modules/image/imagemodule.hpp>
#include <objects/imagedata_ext.hpp>

#include <modules/filesystem/physfs/filesystem.hpp>

using namespace love;

/* set/get pixel functions */

static void setPixelRGBA8(const Color& color, ImageData<>::Pixel* pixel)
{
    pixel->rgba8[0] = static_cast<uint8_t>(std::clamp<float>(color.r, 0, 1) * 0xFF);
    pixel->rgba8[1] = static_cast<uint8_t>(std::clamp<float>(color.g, 0, 1) * 0xFF);
    pixel->rgba8[2] = static_cast<uint8_t>(std::clamp<float>(color.b, 0, 1) * 0xFF);
    pixel->rgba8[3] = static_cast<uint8_t>(std::clamp<float>(color.a, 0, 1) * 0xFF);
}

static void getPixelRGBA8(const ImageData<>::Pixel* pixel, Color& color)
{
    color.r = pixel->rgba8[0] / 0xFF;
    color.g = pixel->rgba8[1] / 0xFF;
    color.b = pixel->rgba8[2] / 0xFF;
    color.a = pixel->rgba8[3] / 0xFF;
}

static void setPixelRGBA16(const Color& color, ImageData<>::Pixel* pixel)
{
    pixel->rgba16[0] = static_cast<uint16_t>(std::clamp<float>(color.r, 0, 1) * 0xFFFF + 0.5f);
    pixel->rgba16[1] = static_cast<uint16_t>(std::clamp<float>(color.b, 0, 1) * 0xFFFF + 0.5f);
    pixel->rgba16[2] = static_cast<uint16_t>(std::clamp<float>(color.g, 0, 1) * 0xFFFF + 0.5f);
    pixel->rgba16[3] = static_cast<uint16_t>(std::clamp<float>(color.a, 0, 1) * 0xFFFF + 0.5f);
}

static void getPixelRGBA16(const ImageData<>::Pixel* pixel, Color& color)
{
    color.r = pixel->rgba16[0] / 0xFFFF;
    color.g = pixel->rgba16[1] / 0xFFFF;
    color.b = pixel->rgba16[2] / 0xFFFF;
    color.a = pixel->rgba16[3] / 0xFFFF;
}

static ImageData<>::PixelSetFunction getPixelSetFunction(PixelFormat format)
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

static ImageData<>::PixelGetFunction getPixelGetFunction(PixelFormat format)
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

ImageData<Console::HAC>::ImageData(Data* data) : ImageData<>(PIXELFORMAT_UNKNOWN, 0, 0)
{
    this->Decode(data);
}

ImageData<Console::HAC>::~ImageData()
{}

void ImageData<Console::HAC>::Decode(Data* data)
{
    FormatHandler* formatDecoder = nullptr;
    FormatHandler::DecodedImage image {};

    auto* module = Module::GetInstance<ImageModule>(Module::M_IMAGE);

    if (!module)
        throw love::Exception("love.image must be loaded in order to decode ImageData.");

    for (auto* handler : module->GetFormatHandlers())
    {
        if (handler->CanDecode(data))
        {
            formatDecoder = handler;
            break;
        }
    }

    if (formatDecoder)
        image = formatDecoder->Decode(data);

    if (image.data == nullptr)
    {
        auto* fileData = (FileData*)data;

        if (fileData)
        {
            const auto* name = fileData->GetFilename().c_str();
            throw love::Exception(
                "Could not decode file '%s' to ImageData: unsupported file format.", name);
        }
        else
            throw love::Exception(
                "Could not decode data to ImageData: unsupported encoded format.");
    }

    const auto expectedSize =
        love::GetPixelFormatSliceSize(image.format, image.width, image.height);

    if (image.size != expectedSize)
    {
        decoder->FreeRawPixels(image.data);
        throw love::Exception("Could not decode image!");
    }

    this->width  = image.width;
    this->height = image.height;

    this->data.reset(image.data);
    this->decoder = formatDecoder;

    this->pixelGetFunction = getPixelGetFunction(this->format);
    this->pixelSetFunction = getPixelSetFunction(this->format);
}

FileData* ImageData<Console::HAC>::Encode(FormatHandler::EncodedFormat encodedFormat,
                                          std::string filename, bool writeFile)
{
    FormatHandler* encoder = nullptr;

    FormatHandler::EncodedImage encoded {};
    FormatHandler::DecodedImage raw {};

    raw.width  = this->width;
    raw.height = this->height;
    raw.size   = this->GetSize();
    raw.data   = this->data.get();
    raw.format = this->format;

    auto* module = Module::GetInstance<ImageModule>(Module::M_IMAGE);

    if (!module)
        throw love::Exception("love.image must be loaded in order to encode ImageData.");

    for (auto* handler : module->GetFormatHandlers())
    {
        if (handler->CanEncode(this->format, encodedFormat))
        {
            encoder = handler;
            break;
        }
    }

    if (encoder)
    {
        std::unique_lock lock(this->mutex);
        encoded = encoder->Encode(raw, encodedFormat);
    }

    if (!encoder || encoded.data == nullptr)
    {
        throw love::Exception("No suitable image encoder for the %s pixel format.",
                              love::GetPixelFormatName(this->format));
    }

    FileData* data = nullptr;

    try
    {
        data = new FileData(encoded.size, filename);
    }
    catch (love::Exception&)
    {
        encoder->FreeEncodedImage(encoded.data);
        throw;
    }

    std::memcpy(data->GetData(), encoded.data, encoded.size);

    if (writeFile)
    {
        auto* filesystem = Module::GetInstance<Filesystem>(Module::M_FILESYSTEM);

        if (!filesystem)
        {
            data->Release();
            throw love::Exception(
                "love.filesystem must be loaded in order to write encoded ImageData to a file.");
        }

        try
        {
            filesystem->Write(filename.c_str(), data->GetData(), data->GetSize());
        }
        catch (love::Exception&)
        {
            data->Release();
            throw;
        }
    }

    return data;
}

void ImageData<Console::HAC>::Create(int width, int height, PixelFormat format, void* data)
{
    ImageData<Console::ALL>::Create(width, height, format, data);

    this->pixelGetFunction = getPixelGetFunction(this->format);
    this->pixelSetFunction = getPixelSetFunction(this->format);

    this->decoder = nullptr;
}

void ImageData<Console::HAC>::GetPixel(int x, int y, Color& color)
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to get out-of-range pixel!");

    size_t pixelSize   = this->GetPixelSize();
    const Pixel* pixel = (const Pixel*)(this->data.get() + ((y * this->width + x) * pixelSize));

    if (this->pixelGetFunction == nullptr)
        throw love::Exception("Unhandled pixel format %d in ImageData::setPixel", format);

    std::unique_lock lock(this->mutex);
    this->pixelGetFunction(pixel, color);
}

void ImageData<Console::HAC>::SetPixel(int x, int y, const Color& color)
{
    if (!this->Inside(x, y))
        throw love::Exception("Attempt to set out-of-range pixel!");

    size_t pixelSize = this->GetPixelSize();
    Pixel* pixel     = (Pixel*)(this->data.get() + ((y * this->width + x) * pixelSize));

    if (this->pixelSetFunction == nullptr)
        throw love::Exception("Unhandled pixel format %d in ImageData::setPixel", this->format);

    std::unique_lock lock(this->mutex);
    this->pixelSetFunction(color, pixel);
}

static float clamp01(float x)
{
    return std::clamp(x, 0.0f, 1.0f);
}

/* pasting functions */

union Row
{
    uint8_t* u8;
    uint16_t* u16;
    // float16_t* f16;
    float_t* f32;
};

static void pasteRGBA8toRGBA16(Row src, Row dst, int w)
{
    for (int i = 0; i < w * 4; i++)
        dst.u16[i] = (uint16_t)src.u8[i] << 8u;
}

static void pasteRGBA16toRGBA8(Row src, Row dst, int w)
{
    for (int i = 0; i < w * 4; i++)
        dst.u8[i] = src.u16[i] >> 8u;
}

void ImageData<Console::HAC>::Paste(ImageData* sourceData, int x, int y, Rect& paste)
{
    PixelFormat sourceFormat = sourceData->GetFormat();
    PixelFormat destFormat   = this->GetFormat();

    int sourceWidth  = sourceData->GetWidth();
    int sourceHeight = sourceData->GetHeight();

    int destinationWidth  = this->GetWidth();
    int destinationHeight = this->GetHeight();

    size_t sourcePixelSize      = sourceData->GetPixelSize();
    size_t destinationPixelSize = this->GetPixelSize();

    bool outsideSourceBounds = paste.x >= sourceWidth || paste.x + paste.w < 0 ||
                               paste.y >= sourceHeight || paste.y + paste.h < 0;

    bool outsideDestinationBounds =
        x >= destinationWidth || x + paste.w < 0 || y >= destinationHeight || y + paste.h < 0;

    if (outsideSourceBounds || outsideDestinationBounds)
        return;

    if (x < 0)
    {
        paste.w += x;
        paste.x -= x;
        x = 0;
    }

    if (y < 0)
    {
        paste.h += y;
        paste.y -= y;
        y = 0;
    }

    if (paste.x < 0)
    {
        paste.w += paste.x;
        x -= paste.x;
        paste.x = 0;
    }

    if (paste.y < 0)
    {
        paste.h += paste.y;
        y -= paste.y;
        paste.y = 0;
    }

    if (x + paste.w > destinationWidth)
        paste.w = destinationWidth - x;

    if (y + paste.h > destinationHeight)
        paste.h = destinationHeight - y;

    if (paste.x + paste.w > sourceWidth)
        paste.w = sourceWidth - paste.x;

    if (paste.y + paste.h > sourceHeight)
        paste.h = sourceHeight - paste.y;

    std::unique_lock lockTwo(sourceData->mutex);
    std::unique_lock lockOne(this->mutex);

    uint8_t* source      = (uint8_t*)sourceData->GetData();
    uint8_t* destination = (uint8_t*)this->GetData();

    const auto getFunction = sourceData->pixelGetFunction;
    const auto setFunction = this->pixelSetFunction;

    const bool isSameFormat = sourceFormat == destFormat;
    if (isSameFormat && (paste.w == destinationWidth && paste.h == destinationHeight &&
                         destinationHeight == sourceHeight))
    {
        std::memcpy(destination, source, sourcePixelSize * paste.w * paste.h);
    }
    else if (paste.w > 0)
    {
        // Otherwise, copy each row individually.
        for (int i = 0; i < paste.h; i++)
        {
            Row sourceRow = { source + (paste.x + (i + paste.y) * sourceWidth) * sourcePixelSize };
            Row destRow = { destination + (x + (i + y) * destinationWidth) * destinationPixelSize };

            if (isSameFormat)
                memcpy(destRow.u8, sourceRow.u8, sourcePixelSize * paste.w);
            else if (sourceFormat == PIXELFORMAT_RGBA8_UNORM &&
                     destFormat == PIXELFORMAT_RGBA16_UNORM)
            {
                pasteRGBA8toRGBA16(sourceRow, destRow, paste.w);
            }
            else if (sourceFormat == PIXELFORMAT_RGBA16_UNORM &&
                     destFormat == PIXELFORMAT_RGBA8_UNORM)
            {
                pasteRGBA16toRGBA8(sourceRow, destRow, paste.w);
            }
            else if (getFunction && setFunction)
            {
                Color color {};

                for (int _x = 0; x < paste.w; x++)
                {
                    auto sourcePixel = (const Pixel*)(sourceRow.u8 + _x * sourcePixelSize);
                    auto destPixel   = (Pixel*)(destRow.u8 + _x * destinationPixelSize);

                    getFunction(sourcePixel, color);
                    setFunction(color, destPixel);
                }
            }
            else if (getFunction == nullptr)
            {
                throw love::Exception("ImageData:paste does not currently support converting from "
                                      "the %s pixel format.",
                                      GetPixelFormatName(sourceFormat));
            }
            else
            {
                throw love::Exception(
                    "ImageData:paste does not currently support converting to the %s pixel format.",
                    GetPixelFormatName(destFormat));
            }
        }
    }
}
