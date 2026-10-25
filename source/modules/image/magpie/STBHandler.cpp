#include "common/Color.hpp"
#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/pixelformat.hpp"
#include "modules/image/FormatHandler.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <limits>
#include <vector>

#include "modules/image/magpie/STBHandler.hpp"

#if defined(__3DS__)
    #include "modules/image/magpie/TileImage.hpp"
#endif

static void love_STBIAssert(bool test, const char* teststr)
{
    if (!test)
        throw love::Exception("Could not decode image (stb_image assertion '{:s}' failed)", teststr);
}

#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#define STBI_ONLY_HDR
#define STBI_NO_STDIO
#define STBI_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(A) love_STBIAssert((A), #A)
#if defined(__WIIU__)
#define STBI_NO_THREAD_LOCALS
#endif
#include "stb_image.h"

#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <cstdlib>

namespace love
{
    struct STBWriteBuffer
    {
        uint8_t* data = nullptr;
        size_t size = 0;
        size_t capacity = 0;
        bool failed = false;
    };

    static void stbWriteCallback(void* context, void* data, int size)
    {
        auto* buffer = (STBWriteBuffer*)context;
        if (size <= 0 || buffer->failed)
            return;

        const auto bytes = (size_t)size;
        if (bytes > std::numeric_limits<size_t>::max() - buffer->size)
        {
            buffer->failed = true;
            return;
        }

        const auto required = buffer->size + bytes;
        if (required > buffer->capacity)
        {
            size_t capacity = buffer->capacity == 0 ? 4096 : buffer->capacity;
            while (capacity < required)
            {
                if (capacity > std::numeric_limits<size_t>::max() / 2)
                {
                    capacity = required;
                    break;
                }
                capacity *= 2;
            }

            auto* resized = (uint8_t*)std::realloc(buffer->data, capacity);
            if (resized == nullptr)
            {
                buffer->failed = true;
                return;
            }

            buffer->data = resized;
            buffer->capacity = capacity;
        }

        std::memcpy(buffer->data + buffer->size, data, bytes);
        buffer->size = required;
    }

    static_assert(sizeof(Color32) == 4, "sizeof(Color32) must be equal to 4 bytes!");

    bool STBHandler::canDecode(Data* data) const
    {
        int w    = 0;
        int h    = 0;
        int comp = 0;

        int status = stbi_info_from_memory((const stbi_uc*)data->getData(), data->getSize(), &w, &h, &comp);

        return status == 1 && w > 0 && h > 0;
    }

    bool STBHandler::canEncode(PixelFormat rawFormat, EncodedFormat encodedFormat) const
    {
        return (encodedFormat == ENCODED_TGA || encodedFormat == ENCODED_JPG) &&
               rawFormat == PIXELFORMAT_RGBA8_UNORM;
    }

    FormatHandler::DecodedImage STBHandler::decode(Data* data) const
    {
        DecodedImage image {};

        const auto* buffer = (const stbi_uc*)data->getData();
        const auto len     = (int)data->getSize();
        int comp           = 0;

        if (stbi_is_hdr_from_memory(buffer, len))
        {
            image.data = (uint8_t*)stbi_loadf_from_memory(buffer, len, &image.width, &image.height, &comp, 4);
            image.size = image.width * image.height * 4 * sizeof(float);
            image.format = PIXELFORMAT_RGBA32_FLOAT;
        }
        else
        {
            image.data   = stbi_load_from_memory(buffer, len, &image.width, &image.height, &comp, 4);
            image.size   = image.width * image.height * 4;
            image.format = PIXELFORMAT_RGBA8_UNORM;
        }

        if (image.data == nullptr || image.width <= 0 || image.height <= 0)
        {
            const char* error = stbi_failure_reason();
            if (error == nullptr)
                error = "unknown error";

            throw love::Exception("Could not decode image with stb_image ({:s}).", error);
        }

#if defined(__3DS__)
        size_t imageSize = image.size / 4;
        for (size_t i = 0; i < imageSize; i++) {
            unsigned char *p = image.data + 4 * i;
            unsigned char r = p[0], g = p[1], b = p[2], a = p[3];
            p[0] = a;
            p[1] = b;
            p[2] = g;
            p[3] = r;
        }
        
        tileImage(image, stbi_image_free);
#endif

        return image;
    }

    FormatHandler::EncodedImage STBHandler::encode(const DecodedImage& image, EncodedFormat encodedFormat) const
    {
        if (!this->canEncode(image.format, encodedFormat))
            throw love::Exception("Cannot encode image (unsupported format).");

        if (encodedFormat == ENCODED_JPG)
        {
            if (image.width <= 0 || image.height <= 0 || image.data == nullptr)
                throw love::Exception("Cannot encode JPG image with these dimensions.");

            const auto maxPixelCount = (size_t)std::numeric_limits<int>::max() / 3;
            if ((size_t)image.width > maxPixelCount / (size_t)image.height)
                throw love::Exception("JPG image is too large to encode.");

            const size_t pixelCount = size_t(image.width) * image.height;
            if (pixelCount > std::numeric_limits<size_t>::max() / 4 || image.size < pixelCount * 4)
                throw love::Exception("Invalid pixel data for JPG encoding.");

            std::vector<uint8_t> rgb(pixelCount * 3);
#if defined(__3DS__)
            auto linearData = untileImage(image);
            const auto* source = linearData.get();
#else
            const auto* source = image.data;
#endif

            for (size_t i = 0; i < pixelCount; i++)
            {
                const auto sourceOffset = i * 4;
                const auto targetOffset = i * 3;
#if defined(__3DS__)
                rgb[targetOffset] = source[sourceOffset + 3];
                rgb[targetOffset + 1] = source[sourceOffset + 2];
                rgb[targetOffset + 2] = source[sourceOffset + 1];
#else
                rgb[targetOffset] = source[sourceOffset];
                rgb[targetOffset + 1] = source[sourceOffset + 1];
                rgb[targetOffset + 2] = source[sourceOffset + 2];
#endif
            }

            STBWriteBuffer output;
            const int success = stbi_write_jpg_to_func(stbWriteCallback, &output, image.width, image.height, 3,
                                                       rgb.data(), 90);
            if (!success || output.failed || output.size == 0)
            {
                std::free(output.data);
                throw love::Exception("Could not encode JPG image.");
            }

            EncodedImage encoded {};
            try
            {
                encoded.data = new uint8_t[output.size];
            }
            catch (std::exception&)
            {
                std::free(output.data);
                throw love::Exception(E_OUT_OF_MEMORY);
            }

            std::memcpy(encoded.data, output.data, output.size);
            encoded.size = output.size;
            std::free(output.data);
            return encoded;
        }

        if (image.width <= 0 || image.height <= 0 || image.width > 65535 || image.height > 65535)
            throw love::Exception("Cannot encode TGA image with these dimensions.");

        const size_t maxPixelCount = (std::numeric_limits<size_t>::max() - STBHandler::HEADER_LENGTH) /
                                     STBHandler::BPP;
        if (size_t(image.width) > maxPixelCount / size_t(image.height))
            throw love::Exception("TGA image is too large to encode.");

        EncodedImage encoded {};
        const size_t pixelSize = size_t(image.width) * image.height * STBHandler::BPP;
        encoded.size = STBHandler::HEADER_LENGTH + pixelSize;

        try
        {
            encoded.data = new uint8_t[encoded.size];
        }
        catch (std::exception&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        // here's the header for the Targa file format.
        encoded.data[0] = 0;                     // ID field size
        encoded.data[1] = 0;                     // colormap type
        encoded.data[2] = 2;                     // image type
        encoded.data[3] = encoded.data[4] = 0;   // colormap start
        encoded.data[5] = encoded.data[6] = 0;   // colormap length
        encoded.data[7]                   = 0;   // colormap bits
        encoded.data[8] = encoded.data[9] = 0;   // x origin
        encoded.data[10] = encoded.data[11] = 0; // y origin
        // Targa is little endian, so:
        encoded.data[12] = image.width & 255;   // least significant byte of width
        encoded.data[13] = image.width >> 8;    // most significant byte of width
        encoded.data[14] = image.height & 255;  // least significant byte of height
        encoded.data[15] = image.height >> 8;   // most significant byte of height
        encoded.data[16] = STBHandler::BPP * 8; // bits per pixel
        encoded.data[17] = 0x28;                // top-left origin and 8 alpha bits

#if defined(__3DS__)
        auto linearData = untileImage(image);
        auto* source = linearData.get();

        for (size_t i = 0; i < pixelSize; i += STBHandler::BPP)
        {
            uint8_t r = source[i];
            uint8_t g = source[i + 1];
            source[i] = source[i + 3];
            source[i + 1] = source[i + 2];
            source[i + 2] = g;
            source[i + 3] = r;
        }
#else
        const auto* source = image.data;
#endif

        std::memcpy(encoded.data + STBHandler::HEADER_LENGTH, source, pixelSize);

        auto* pixels = encoded.data + STBHandler::HEADER_LENGTH;
        for (size_t i = 0; i < pixelSize; i += STBHandler::BPP)
        {
            uint8_t r = pixels[i];
            pixels[i] = pixels[i + 2];
            pixels[i + 2] = r;
        }

        return encoded;
    }

    void STBHandler::freeRawPixels(uint8_t* memory)
    {
#if defined(__3DS__)
        delete[] memory;
#else
        stbi_image_free(memory);
#endif
    }

    void STBHandler::freeEncodedImage(uint8_t* memory)
    {
        delete[] memory;
    }
} // namespace love
