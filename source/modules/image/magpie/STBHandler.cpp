#include "common/Color.hpp"
#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/pixelformat.hpp"
#include "modules/image/FormatHandler.hpp"
#include <cstdint>
#include <cstring>
#include <exception>

#include "modules/image/magpie/STBHandler.hpp"

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
#include "stb_image.h"

#include <cstdlib>

namespace love
{
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
        return encodedFormat == ENCODED_TGA && rawFormat == PIXELFORMAT_RGBA8_UNORM;
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

        return image;
    }

    FormatHandler::EncodedImage STBHandler::encode(const DecodedImage& image, EncodedFormat encodedFormat) const
    {
        if (!this->canEncode(image.format, encodedFormat))
            throw love::Exception("Cannot encode image (unsupported format).");

        EncodedImage encoded {};
        encoded.size = (image.width * image.height * STBHandler::BPP);

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
        encoded.data[7]                   = 32;  // colormap bits
        encoded.data[8] = encoded.data[9] = 0;   // x origin
        encoded.data[10] = encoded.data[11] = 0; // y origin
        // Targa is little endian, so:
        encoded.data[12] = image.width & 255;   // least significant byte of width
        encoded.data[13] = image.width >> 8;    // most significant byte of width
        encoded.data[14] = image.height & 255;  // least significant byte of height
        encoded.data[15] = image.height >> 8;   // most significant byte of height
        encoded.data[16] = STBHandler::BPP * 8; // bits per pixel
        encoded.data[17] = 0x20;                // descriptor bits (flip bits: 0x10 horizontal, 0x20 vertical)

        const auto size = image.width * image.height * STBHandler::BPP;
        std::memcpy(encoded.data + STBHandler::HEADER_LENGTH, image.data, size);

        auto* pixels = (Color32*)encoded.data + STBHandler::HEADER_LENGTH;
        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                uint8_t r                     = pixels[y * image.width + x].r;
                uint8_t b                     = pixels[y * image.width + x].b;
                pixels[y * image.width + x].r = b;
                pixels[y * image.width + x].b = r;
            }
        }

        return encoded;
    }

    void STBHandler::freeRawPixels(uint8_t* memory)
    {
        stbi_image_free(memory);
    }

    void STBHandler::freeEncodedImage(uint8_t* memory)
    {
        delete[] memory;
    }
} // namespace love
