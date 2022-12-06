#include <modules/image/imagemodule.hpp>
#include <objects/imagedata_ext.hpp>

using namespace love;

static void setPixelRGBA8(const Color& color, ImageData<Console::CTR>::Pixel* pixel)
{
    pixel->packed32 = color.abgr();
}

static void getPixelRGBA8(const ImageData<Console::CTR>::Pixel* pixel, Color& color)
{
    color = Color(pixel->packed32);
}

ImageData<Console::CTR>::ImageData(Data* data) : ImageData<>(PIXELFORMAT_UNKNOWN, 0, 0)
{
    this->Decode(data);
}

ImageData<Console::CTR>::~ImageData()
{}

void ImageData<Console::CTR>::Decode(Data* data)
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
    this->decoder = decoder;

    this->pixelGetFunction = getPixelRGBA8;
    this->pixelSetFunction = setPixelRGBA8;
}

FileData* ImageData<Console::CTR>::Encode(FormatHandler::EncodedFormat encodedFormat,
                                          std::string_view filename, bool writeFile) const
{
    throw love::Exception("No suitable image encoder for the %s pixel format.",
                          love::GetPixelFormatName(this->format));

    return nullptr;
}

/*
** ImageData::Create will internally make NextPo2 sized pixel data for citro3d textures
** ImageData's c'tors that take with/height will assign non-Po2 sizes to those variables
**   - This ensures that we can't possibly mess up this information in the future
*/
void ImageData<Console::CTR>::Create(int width, int height, PixelFormat format, void* data)
{
    ImageData<Console::ALL>::Create(NextPo2(width), NextPo2(height), format, data);

    this->pixelSetFunction = setPixelRGBA8;
    this->pixelGetFunction = getPixelRGBA8;

    this->decoder = nullptr;
}

void ImageData<Console::CTR>::Paste(ImageData* sourceData, int x, int y, Rect& paste)
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

    unsigned sourcePowTwo      = NextPo2(sourceData->width);
    unsigned destinationPowTwo = NextPo2(this->width);

    for (int _y = 0; _y < std::min(paste.h, destinationHeight - y); _y++)
    {
        for (int _x = 0; _x < std::min(paste.w, destinationWidth - x); _x++)
        {
            unsigned srcIndex = coordToIndex(sourcePowTwo, (paste.x + _x), (paste.y + _y));
            unsigned dstIndex = coordToIndex(destinationPowTwo, (x + _x), (y + _y));

            Color color {};

            const Pixel* srcPixel = reinterpret_cast<const Pixel*>((uint32_t*)source + srcIndex);
            getFunction(srcPixel, color);

            Pixel* dstPixel = reinterpret_cast<Pixel*>((uint32_t*)this->data.get() + dstIndex);
            setFunction(color, dstPixel);
        }
    }
}

Color ImageData<Console::CTR>::GetPixel(int x, int y)
{
    Color color {};
    this->GetPixel(x, y, color);

    return color;
}

void ImageData<Console::CTR>::GetPixel(int x, int y, Color& color)
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

void ImageData<Console::CTR>::SetPixel(int x, int y, const Color& color)
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
