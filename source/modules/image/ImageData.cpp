#include "common/Console.hpp"
#include "common/math.hpp"

#include "modules/filesystem/physfs/Filesystem.hpp"
#include "modules/image/Image.hpp"
#include "modules/image/ImageData.hpp"

#define E_PIXELFORMAT_NOT_SUPPORTED "ImageData does not support the {:s} pixel format."
#define E_LOVE_IMAGE_NOT_LOADED     "love.image must be loaded in order to {:s} an ImageData."
#define E_LOVE_FILESYSTEM_NOT_LOADED \
    "love.filesystem must be loaded in order to write an encoded ImageData to a file."
#define E_CANNOT_CONVERT_FROM_FORMAT \
    "ImageData:paste does not currently support converting from the {:s} pixel format."
#define E_CANNOT_CONVERT_TO_FORMAT \
    "ImageData:paste does not currently support converting to the {:s} pixel format."
#define E_OUT_OF_RANGE_PIXEL "Attempt to set out-of-range pixel at ({:d}, {:d})."

namespace love
{
    Type ImageData::type("ImageData", &Data::type);

    ImageData::ImageData(Data* data) : ImageDataBase(PIXELFORMAT_UNKNOWN, 0, 0)
    {
        this->decode(data);
    }

    ImageData::ImageData(int width, int height, PixelFormat format) : ImageDataBase(format, width, height)
    {
        if (!ImageData::validPixelFormat(format))
        {
            const char* name = love::getConstant(format);
            throw love::Exception(E_PIXELFORMAT_NOT_SUPPORTED, name);
        }

        this->create(width, height, format);
        std::memset(this->data, 0, this->getSize());
    }

    ImageData::ImageData(int width, int height, PixelFormat format, void* data, bool own) :
        ImageDataBase(format, width, height)
    {
        if (!ImageData::validPixelFormat(format))
        {
            const char* name = love::getConstant(format);
            throw love::Exception(E_PIXELFORMAT_NOT_SUPPORTED, name);
        }

        if (own)
            this->data = (uint8_t*)data;
        else
            this->create(width, height, format, data);
    }

    ImageData::ImageData(const ImageData& other) : ImageDataBase(other.format, other.width, other.height)
    {
        this->create(other.width, other.height, other.format, other.data);
    }

    ImageData::~ImageData()
    {
        if (this->decodeHandler.get())
            this->decodeHandler->freeRawPixels(this->data);
        else
            delete[] this->data;
    }

    ImageData* ImageData::clone() const
    {
        return new ImageData(*this);
    }

    static constexpr bool needsPowTwo = (Console::is(Console::CTR) ? true : false);

    void ImageData::create(int width, int height, PixelFormat format, void* data)
    {
        const auto dataSize = love::getPixelFormatSliceSize(format, width, height, needsPowTwo);

        try
        {
            this->data = new uint8_t[dataSize];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        if (data)
        {
            if constexpr (Console::is(Console::CTR))
            {
                switch (format)
                {
                    case PIXELFORMAT_RGB565_UNORM:
                    case PIXELFORMAT_RGB5A1_UNORM:
                    case PIXELFORMAT_RGBA4_UNORM:
                        this->copyBytesTiled<uint16_t>(data, width, height);
                        break;
                    case PIXELFORMAT_LA8_UNORM:
                        this->copyBytesTiled<uint8_t>(data, width, height);
                        break;
                    case PIXELFORMAT_RGBA8_UNORM:
                    default:
                        this->copyBytesTiled<uint32_t>(data, width, height);
                        break;
                }
            }
            else
                std::memcpy(this->data, data, dataSize);
        }

        this->decodeHandler = nullptr;
        this->format        = format;

        this->pixelSetFunction = ImageData::getPixelSetFunction(format);
        this->pixelGetFunction = ImageData::getPixelGetFunction(format);
    }

    void ImageData::decode(Data* data)
    {
        FormatHandler* decoder = nullptr;
        FormatHandler::DecodedImage image {};

        auto* module = Module::getInstance<Image>(Module::M_IMAGE);

        if (module == nullptr)
            throw love::Exception(E_LOVE_IMAGE_NOT_LOADED, "decode");

        for (auto* handler : module->getFormatHandlers())
        {
            if (handler->canDecode(data))
            {
                decoder = handler;
                break;
            }
        }

        if (decoder)
            image = decoder->decode(data);

        if (image.data == nullptr)
            throw love::Exception("Could not decode to ImageData: unsupported format");

        if (image.size != love::getPixelFormatSliceSize(image.format, image.width, image.height, false))
        {
            decoder->freeRawPixels(image.data);
            throw love::Exception("Could not convert image!");
        }

        if (this->decodeHandler)
            decodeHandler->freeRawPixels(this->data);
        else
            delete[] this->data;

        image.format = getLinearPixelFormat(image.format);

        this->width  = image.width;
        this->height = image.height;
        this->data   = image.data;
        this->format = image.format;

        this->decodeHandler = decoder;

        this->pixelSetFunction = ImageData::getPixelSetFunction(this->format);
        this->pixelGetFunction = ImageData::getPixelGetFunction(this->format);
    }

    FileData* ImageData::encode(FormatHandler::EncodedFormat encodedFormat, const char* filename,
                                bool writeFile) const
    {
        FormatHandler* encoder = nullptr;

        FormatHandler::EncodedImage image {};
        FormatHandler::DecodedImage rawImage {};

        rawImage.width  = this->width;
        rawImage.height = this->height;
        rawImage.size   = this->getSize();
        rawImage.data   = this->data;
        rawImage.format = this->format;

        auto* module = Module::getInstance<Image>(Module::M_IMAGE);

        if (module == nullptr)
            throw love::Exception(E_LOVE_IMAGE_NOT_LOADED, "encode");

        for (auto* handler : module->getFormatHandlers())
        {
            if (handler->canEncode(this->format, encodedFormat))
            {
                encoder = handler;
                break;
            }
        }

        if (encoder != nullptr)
            image = encoder->encode(rawImage, encodedFormat);

        if (encoder == nullptr || image.data == nullptr)
        {
            const char* formatName = love::getConstant(this->format);
            throw love::Exception("No suitable image encoder for the {:s} pixel format.", formatName);
        }

        FileData* fileData = nullptr;

        try
        {
            fileData = new FileData(image.size, filename);
        }
        catch (love::Exception&)
        {
            encoder->freeEncodedImage(image.data);
            throw;
        }

        std::memcpy(fileData->getData(), image.data, image.size);
        encoder->freeEncodedImage(image.data);

        if (writeFile)
        {
            auto* filesystem = Module::getInstance<Filesystem>(Module::M_FILESYSTEM);

            if (filesystem == nullptr)
                throw love::Exception(E_LOVE_FILESYSTEM_NOT_LOADED);

            try
            {
                filesystem->write(filename, fileData->getData(), fileData->getSize());
            }
            catch (love::Exception&)
            {
                fileData->release();
                throw;
            }
        }

        return fileData;
    }

    size_t ImageData::getSize() const
    {
        return size_t(this->getWidth() * this->getHeight()) * this->getPixelSize();
    }

    void* ImageData::getData() const
    {
        return this->data;
    }

    bool ImageData::inside(int x, int y) const
    {
        return x >= 0 && x < this->getWidth() && y >= 0 && y < this->getHeight();
    }

    static void setPixelR8(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba8[0] = (uint8_t)(std::clamp(color.r, 0.0f, 1.0f) * 255.0f + 0.5f);
    }

    static void setPixelRG8(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba8[0] = (uint8_t)(std::clamp(color.r, 0.0f, 1.0f) * 255.0f + 0.5f);
        pixel->rgba8[1] = (uint8_t)(std::clamp(color.g, 0.0f, 1.0f) * 255.0f + 0.5f);
    }

    static void setPixelRGBA8(const Color& color, ImageData::Pixel* pixel)
    {
        if (Console::is(Console::CTR))
        {
            pixel->packed32 = color.abgr();
            return;
        }

        pixel->rgba8[0] = (uint8_t)(std::clamp(color.r, 0.0f, 1.0f) * 255.0f + 0.5f);
        pixel->rgba8[1] = (uint8_t)(std::clamp(color.g, 0.0f, 1.0f) * 255.0f + 0.5f);
        pixel->rgba8[2] = (uint8_t)(std::clamp(color.b, 0.0f, 1.0f) * 255.0f + 0.5f);
        pixel->rgba8[3] = (uint8_t)(std::clamp(color.a, 0.0f, 1.0f) * 255.0f + 0.5f);
    }

    static void setPixelR16(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba16[0] = (uint16_t)(std::clamp(color.r, 0.0f, 1.0f) * 65535.0f + 0.5f);
    }

    static void setPixelRG16(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba16[0] = (uint16_t)(std::clamp(color.r, 0.0f, 1.0f) * 65535.0f + 0.5f);
        pixel->rgba16[1] = (uint16_t)(std::clamp(color.g, 0.0f, 1.0f) * 65535.0f + 0.5f);
    }

    static void setPixelRGBA16(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba16[0] = (uint16_t)(std::clamp(color.r, 0.0f, 1.0f) * 65535.0f + 0.5f);
        pixel->rgba16[1] = (uint16_t)(std::clamp(color.g, 0.0f, 1.0f) * 65535.0f + 0.5f);
        pixel->rgba16[2] = (uint16_t)(std::clamp(color.b, 0.0f, 1.0f) * 65535.0f + 0.5f);
        pixel->rgba16[3] = (uint16_t)(std::clamp(color.a, 0.0f, 1.0f) * 65535.0f + 0.5f);
    }

    static void setPixelR16F(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba16f[0] = float32to16(color.r);
    }

    static void setPixelRG16F(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba16f[0] = float32to16(color.r);
        pixel->rgba16f[1] = float32to16(color.g);
    }

    static void setPixelRGBA16F(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba16f[0] = float32to16(color.r);
        pixel->rgba16f[1] = float32to16(color.g);
        pixel->rgba16f[2] = float32to16(color.b);
        pixel->rgba16f[3] = float32to16(color.a);
    }

    static void setPixelR32F(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba32f[0] = color.r;
    }

    static void setPixelRG32F(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba32f[0] = color.r;
        pixel->rgba32f[1] = color.g;
    }

    static void setPixelRGBA32F(const Color& color, ImageData::Pixel* pixel)
    {
        pixel->rgba32f[0] = color.r;
        pixel->rgba32f[1] = color.g;
        pixel->rgba32f[2] = color.b;
        pixel->rgba32f[3] = color.a;
    }

    static void setPixelRGBA4(const Color& color, ImageData::Pixel* pixel)
    {
        uint16_t red   = (uint16_t)(std::clamp(color.r, 0.0f, 1.0f) * 0xF + 0.5f);
        uint16_t green = (uint16_t)(std::clamp(color.g, 0.0f, 1.0f) * 0xF + 0.5f);
        uint16_t blue  = (uint16_t)(std::clamp(color.b, 0.0f, 1.0f) * 0xF + 0.5f);
        uint16_t alpha = (uint16_t)(std::clamp(color.a, 0.0f, 1.0f) * 0xF + 0.5f);

        pixel->packed16 = (red << 12) | (green << 8) | (blue << 4) | (alpha << 0);
    }

    static void setPixelRGB5A1(const Color& color, ImageData::Pixel* pixel)
    {
        uint16_t red   = (uint16_t)(std::clamp(color.r, 0.0f, 1.0f) * 0x1F + 0.5f);
        uint16_t green = (uint16_t)(std::clamp(color.g, 0.0f, 1.0f) * 0x1F + 0.5f);
        uint16_t blue  = (uint16_t)(std::clamp(color.b, 0.0f, 1.0f) * 0x1F + 0.5f);
        uint16_t alpha = (uint16_t)(std::clamp(color.a, 0.0f, 1.0f) * 0x1 + 0.5f);

        pixel->packed16 = (red << 11) | (green << 6) | (blue << 1) | (alpha << 0);
    }

    static void setPixelRGB565(const Color& color, ImageData::Pixel* pixel)
    {
        uint16_t red   = (uint16_t)(std::clamp(color.r, 0.0f, 1.0f) * 0x1F + 0.5f);
        uint16_t green = (uint16_t)(std::clamp(color.g, 0.0f, 1.0f) * 0x3F + 0.5f);
        uint16_t blue  = (uint16_t)(std::clamp(color.b, 0.0f, 1.0f) * 0x1F + 0.5f);

        pixel->packed16 = (red << 11) | (green << 5) | (blue << 0);
    }

    static void setPixelRGB10A2(const Color& color, ImageData::Pixel* pixel)
    {
        uint32_t red   = (uint32_t)(std::clamp(color.r, 0.0f, 1.0f) * 0x3FF + 0.5f);
        uint32_t green = (uint32_t)(std::clamp(color.g, 0.0f, 1.0f) * 0x3FF + 0.5f);
        uint32_t blue  = (uint32_t)(std::clamp(color.b, 0.0f, 1.0f) * 0x3FF + 0.5f);
        uint32_t alpha = (uint32_t)(std::clamp(color.a, 0.0f, 1.0f) * 0x3 + 0.5f);

        pixel->packed32 = (red << 0) | (green << 10) | (blue << 20) | (alpha << 30);
    }

    static void setPixelRG11B10F(const Color& color, ImageData::Pixel* pixel)
    {
        float11_t red   = float32to11(color.r);
        float11_t green = float32to11(color.g);
        float10_t blue  = float32to10(color.b);

        pixel->packed32 = (red << 0) | (green << 11) | (blue << 22);
    }

    static void getPixelR8(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba8[0] / 255.0f;
        color.g = 0.0f;
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRG8(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba8[0] / 255.0f;
        color.g = pixel->rgba8[1] / 255.0f;
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRGBA8(const ImageData::Pixel* pixel, Color& color)
    {
        if (Console::is(Console::CTR))
        {
            color = Color(pixel->packed32);
            return;
        }

        color.r = pixel->rgba8[0] / 255.0f;
        color.g = pixel->rgba8[1] / 255.0f;
        color.b = pixel->rgba8[2] / 255.0f;
        color.a = pixel->rgba8[3] / 255.0f;
    }

    static void getPixelR16(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba16[0] / 65535.0f;
        color.g = 0.0f;
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRG16(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba16[0] / 65535.0f;
        color.g = pixel->rgba16[1] / 65535.0f;
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRGBA16(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba16[0] / 65535.0f;
        color.g = pixel->rgba16[1] / 65535.0f;
        color.b = pixel->rgba16[2] / 65535.0f;
        color.a = pixel->rgba16[3] / 65535.0f;
    }

    static void getPixelR16F(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = float16to32(pixel->rgba16f[0]);
        color.g = 0.0f;
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRG16F(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = float16to32(pixel->rgba16f[0]);
        color.g = float16to32(pixel->rgba16f[1]);
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRGBA16F(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = float16to32(pixel->rgba16f[0]);
        color.g = float16to32(pixel->rgba16f[1]);
        color.b = float16to32(pixel->rgba16f[2]);
        color.a = float16to32(pixel->rgba16f[3]);
    }

    static void getPixelR32F(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba32f[0];
        color.g = 0.0f;
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRG32F(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba32f[0];
        color.g = pixel->rgba32f[1];
        color.b = 0.0f;
        color.a = 1.0f;
    }

    static void getPixelRGBA32F(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = pixel->rgba32f[0];
        color.g = pixel->rgba32f[1];
        color.b = pixel->rgba32f[2];
        color.a = pixel->rgba32f[3];
    }

    static void getPixelRGBA4(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = ((pixel->packed16 >> 12) & 0xF) / (float)0xF;
        color.g = ((pixel->packed16 >> 8) & 0xF) / (float)0xF;
        color.b = ((pixel->packed16 >> 4) & 0xF) / (float)0xF;
        color.a = ((pixel->packed16 >> 0) & 0xF) / (float)0xF;
    }

    static void getPixelRGB5A1(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = ((pixel->packed16 >> 11) & 0x1F) / (float)0x1F;
        color.g = ((pixel->packed16 >> 6) & 0x1F) / (float)0x1F;
        color.b = ((pixel->packed16 >> 1) & 0x1F) / (float)0x1F;
        color.a = ((pixel->packed16 >> 0) & 0x1) / (float)0x1;
    }

    static void getPixelRGB565(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = ((pixel->packed16 >> 11) & 0x1F) / (float)0x1F;
        color.g = ((pixel->packed16 >> 5) & 0x3F) / (float)0x3F;
        color.b = ((pixel->packed16 >> 0) & 0x1F) / (float)0x1F;
        color.a = 1.0f;
    }

    static void getPixelRGB10A2(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = ((pixel->packed32 >> 0) & 0x3FF) / (float)0x3FF;
        color.g = ((pixel->packed32 >> 10) & 0x3FF) / (float)0x3FF;
        color.b = ((pixel->packed32 >> 20) & 0x3FF) / (float)0x3FF;
        color.a = ((pixel->packed32 >> 30) & 0x3) / (float)0x3;
    }

    static void getPixelRG11B10F(const ImageData::Pixel* pixel, Color& color)
    {
        color.r = float11to32((float11_t)(pixel->packed32 >> 0) & 0x7FF);
        color.g = float11to32((float11_t)(pixel->packed32 >> 11) & 0x7FF);
        color.b = float10to32((float10_t)(pixel->packed32 >> 22) & 0x7FF);
        color.a = 1.0f;
    }

    void ImageData::setPixel(int x, int y, const Color& color)
    {
        if (!this->inside(x, y))
            throw love::Exception(E_OUT_OF_RANGE_PIXEL, x, y);

        Pixel* pixel     = nullptr;
        size_t pixelSize = this->getPixelSize();

        if (Console::is(Console::CTR))
        {
            const auto _width = NextPo2(this->getWidth());
            pixel             = (Pixel*)Color::fromTile(this->data, _width, x, y);
        }
        else
            pixel = (Pixel*)(this->data + ((y * this->width + x) * pixelSize));

        if (this->pixelSetFunction == nullptr)
        {
            throw love::Exception(E_PIXELFORMAT_NOT_SUPPORTED, love::getConstant(this->format));
        }

        this->pixelSetFunction(color, pixel);
    }

    void ImageData::getPixel(int x, int y, Color& color) const
    {
        if (!this->inside(x, y))
            throw love::Exception(E_OUT_OF_RANGE_PIXEL, x, y);

        const Pixel* pixel = nullptr;
        size_t pixelSize   = this->getPixelSize();

        if (Console::is(Console::CTR))
        {
            const auto _width = NextPo2(this->getWidth());
            pixel             = (const Pixel*)Color::fromTile(this->data, _width, x, y);
        }
        else
            pixel = (const Pixel*)(this->data + ((y * this->width + x) * pixelSize));

        this->pixelGetFunction(pixel, color);
    }

    Color ImageData::getPixel(int x, int y) const
    {
        Color color {};
        this->getPixel(x, y, color);

        return color;
    }

    union Row
    {
        uint8_t* u8;
        uint16_t* u16;
        float16_t* f16;
        float* f32;
    };

    static void pasteRGBA8toRGBA16(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.u16[i] = (uint16_t)src.u8[i] << 8u;
    }

    static void pasteRGBA8toRGBA16F(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.f16[i] = float32to16(src.u8[i] / 255.0f);
    }

    static void pasteRGBA8toRGBA32F(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.f32[i] = src.u8[i] / 255.0f;
    }

    static void pasteRGBA16toRGBA8(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.u8[i] = src.u16[i] >> 8u;
    }

    static void pasteRGBA16toRGBA16F(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.f16[i] = float32to16(src.u16[i] / 65535.0f);
    }

    static void pasteRGBA16toRGBA32F(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.f32[i] = src.u16[i] / 65535.0f;
    }

    static float clamp01(const float x)
    {
        return std::clamp(x, 0.0f, 1.0f);
    }

    static void pasteRGBA16FtoRGBA8(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.u8[i] = (uint8_t)(clamp01(float16to32(src.f16[i])) * 255.0f + 0.5f);
    }

    static void pasteRGBA16FtoRGBA16(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.u16[i] = (uint16_t)(clamp01(float16to32(src.f16[i])) * 65535.0f + 0.5f);
    }

    static void pasteRGBA16FtoRGBA32F(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.f32[i] = float16to32(src.f16[i]);
    }

    static void pasteRGBA32FtoRGBA8(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.u8[i] = (uint8_t)(clamp01(src.f32[i]) * 255.0f + 0.5f);
    }

    static void pasteRGBA32FtoRGBA16(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.u16[i] = (uint16_t)(clamp01(src.f32[i]) * 65535.0f + 0.5f);
    }

    static void pasteRGBA32FtoRGBA16F(Row src, Row dst, int w)
    {
        for (int i = 0; i < w * 4; i++)
            dst.f16[i] = float32to16(src.f32[i]);
    }

    void ImageData::paste(ImageData* src, int dx, int dy, int sx, int sy, int sw, int sh)
    {
        const auto destFormat = this->getFormat();
        const auto srcFormat  = src->getFormat();

        int srcWidth  = src->getWidth();
        int srcHeight = src->getHeight();

        int destWidth  = this->getWidth();
        int destHeight = this->getHeight();

        size_t srcPixelSize  = src->getPixelSize();
        size_t destPixelSize = this->getPixelSize();

        if (sx >= srcWidth || sx + sw < 0 || sy >= srcHeight || sy + sh < 0 || dx >= destWidth ||
            dx + sw < 0 || dy >= destHeight || dy + sh < 0)
        {
            return;
        }

        if (dx < 0)
        {
            srcWidth += dx;
            sx -= dx;
            dx = 0;
        }

        if (dy < 0)
        {
            srcHeight += dy;
            sy -= dy;
            dy = 0;
        }

        if (sx < 0)
        {
            srcWidth += sx;
            dx -= sx;
            sx = 0;
        }

        if (sy < 0)
        {
            srcHeight += sy;
            dy -= sy;
            sy = 0;
        }

        if (dx + sw > destWidth)
            sw = destWidth - dx;

        if (dy + sh > destHeight)
            sh = destHeight - dy;

        if (sx + sw > srcWidth)
            sw = srcWidth - sx;

        if (sy + sh > srcHeight)
            sh = srcHeight - sy;

        auto* source = (uint8_t*)src->getData();
        auto* dest   = (uint8_t*)this->getData();

        auto getFunction = src->getPixelGetFunction();
        auto setFunction = this->pixelSetFunction;

        if (srcFormat == destFormat &&
            (sw == destWidth && destWidth == srcWidth && sh == destHeight && destHeight == srcHeight))
        {
            memcpy(dest, source, srcPixelSize * sw * sh);
        }
        else if (sw > 0)
        {
            // Otherwise, copy each row individually.
            for (int i = 0; i < sh; i++)
            {
                Row rowsrc = { source + (sx + (i + sy) * srcWidth) * srcPixelSize };
                Row rowdst = { dest + (dx + (i + dy) * destWidth) * destPixelSize };

                if (srcFormat == destFormat)
                    std::memcpy(rowdst.u8, rowsrc.u8, srcPixelSize * sw);
                else if (srcFormat == PIXELFORMAT_RGBA8_UNORM && destFormat == PIXELFORMAT_RGBA16_UNORM)
                    pasteRGBA8toRGBA16(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA8_UNORM && destFormat == PIXELFORMAT_RGBA16_FLOAT)
                    pasteRGBA8toRGBA16F(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA8_UNORM && destFormat == PIXELFORMAT_RGBA32_FLOAT)
                    pasteRGBA8toRGBA32F(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA16_UNORM && destFormat == PIXELFORMAT_RGBA8_UNORM)
                    pasteRGBA16toRGBA8(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA16_UNORM && destFormat == PIXELFORMAT_RGBA16_FLOAT)
                    pasteRGBA16toRGBA16F(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA16_UNORM && destFormat == PIXELFORMAT_RGBA32_FLOAT)
                    pasteRGBA16toRGBA32F(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA16_FLOAT && destFormat == PIXELFORMAT_RGBA8_UNORM)
                    pasteRGBA16FtoRGBA8(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA16_FLOAT && destFormat == PIXELFORMAT_RGBA16_UNORM)
                    pasteRGBA16FtoRGBA16(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA16_FLOAT && destFormat == PIXELFORMAT_RGBA32_FLOAT)
                    pasteRGBA16FtoRGBA32F(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA32_FLOAT && destFormat == PIXELFORMAT_RGBA8_UNORM)
                    pasteRGBA32FtoRGBA8(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA32_FLOAT && destFormat == PIXELFORMAT_RGBA16_UNORM)
                    pasteRGBA32FtoRGBA16(rowsrc, rowdst, sw);
                else if (srcFormat == PIXELFORMAT_RGBA32_FLOAT && destFormat == PIXELFORMAT_RGBA16_FLOAT)
                    pasteRGBA32FtoRGBA16F(rowsrc, rowdst, sw);
                else if (getFunction != nullptr && setFunction != nullptr)
                {
                    // Slow path: convert src -> Colorf -> dst.
                    Color c;
                    for (int x = 0; x < sw; x++)
                    {
                        auto srcp = (const Pixel*)(rowsrc.u8 + x * srcPixelSize);
                        auto dstp = (Pixel*)(rowdst.u8 + x * destPixelSize);

                        getFunction(srcp, c);
                        setFunction(c, dstp);
                    }
                }
                else if (getFunction == nullptr)
                {
                    const char* name = love::getConstant(srcFormat);
                    throw love::Exception(E_CANNOT_CONVERT_FROM_FORMAT, name);
                }
                else
                {
                    const char* name = love::getConstant(srcFormat);
                    throw love::Exception(E_CANNOT_CONVERT_TO_FORMAT, name);
                }
            }
        }
    }

    size_t ImageData::getPixelSize() const
    {
        return love::getPixelFormatBlockSize(this->format);
    }

    bool ImageData::validPixelFormat(PixelFormat format)
    {
        return isPixelFormatColor(format) && !isPixelFormatCompressed(format);
    }

    ImageData::PixelSetFunction ImageData::getPixelSetFunction(PixelFormat format)
    {
        switch (format)
        {
            case PIXELFORMAT_R8_UNORM:
                return setPixelR8;
            case PIXELFORMAT_RG8_UNORM:
                return setPixelRG8;
            case PIXELFORMAT_RGBA8_UNORM:
                return setPixelRGBA8;
            case PIXELFORMAT_R16_UNORM:
                return setPixelR16;
            case PIXELFORMAT_RG16_UNORM:
                return setPixelRG16;
            case PIXELFORMAT_RGBA16_UNORM:
                return setPixelRGBA16;
            case PIXELFORMAT_R16_FLOAT:
                return setPixelR16F;
            case PIXELFORMAT_RG16_FLOAT:
                return setPixelRG16F;
            case PIXELFORMAT_RGBA16_FLOAT:
                return setPixelRGBA16F;
            case PIXELFORMAT_R32_FLOAT:
                return setPixelR32F;
            case PIXELFORMAT_RG32_FLOAT:
                return setPixelRG32F;
            case PIXELFORMAT_RGBA32_FLOAT:
                return setPixelRGBA32F;
            case PIXELFORMAT_RGBA4_UNORM:
                return setPixelRGBA4;
            case PIXELFORMAT_RGB5A1_UNORM:
                return setPixelRGB5A1;
            case PIXELFORMAT_RGB565_UNORM:
                return setPixelRGB565;
            case PIXELFORMAT_RGB10A2_UNORM:
                return setPixelRGB10A2;
            case PIXELFORMAT_RG11B10_FLOAT:
                return setPixelRG11B10F;
            default:
                return nullptr;
        }
    }

    ImageData::PixelGetFunction ImageData::getPixelGetFunction(PixelFormat format)
    {
        switch (format)
        {
            case PIXELFORMAT_R8_UNORM:
                return getPixelR8;
            case PIXELFORMAT_RG8_UNORM:
                return getPixelRG8;
            case PIXELFORMAT_RGBA8_UNORM:
                return getPixelRGBA8;
            case PIXELFORMAT_R16_UNORM:
                return getPixelR16;
            case PIXELFORMAT_RG16_UNORM:
                return getPixelRG16;
            case PIXELFORMAT_RGBA16_UNORM:
                return getPixelRGBA16;
            case PIXELFORMAT_R16_FLOAT:
                return getPixelR16F;
            case PIXELFORMAT_RG16_FLOAT:
                return getPixelRG16F;
            case PIXELFORMAT_RGBA16_FLOAT:
                return getPixelRGBA16F;
            case PIXELFORMAT_R32_FLOAT:
                return getPixelR32F;
            case PIXELFORMAT_RG32_FLOAT:
                return getPixelRG32F;
            case PIXELFORMAT_RGBA32_FLOAT:
                return getPixelRGBA32F;
            case PIXELFORMAT_RGBA4_UNORM:
                return getPixelRGBA4;
            case PIXELFORMAT_RGB5A1_UNORM:
                return getPixelRGB5A1;
            case PIXELFORMAT_RGB565_UNORM:
                return getPixelRGB565;
            case PIXELFORMAT_RGB10A2_UNORM:
                return getPixelRGB10A2;
            case PIXELFORMAT_RG11B10_FLOAT:
                return getPixelRG11B10F;
            default:
                return nullptr;
        }
    }
} // namespace love
