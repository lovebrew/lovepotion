#include "modules/image/imagemodule.h"

#include "objects/imagedata/handlers/jpghandler.h"
#include "objects/imagedata/handlers/pnghandler.h"
#include "objects/imagedata/handlers/t3xhandler.h"

#include "common/bidirectionalmap.h"
#include "objects/compressedimagedata/handlers/astchandler.h"
#include "objects/compressedimagedata/handlers/ddshandler.h"
#include "objects/compressedimagedata/handlers/pkmhandler.h"

using namespace love;

ImageModule::ImageModule()
{
    this->formatHandlers = {
#if not defined(__3DS__)
        new PNGHandler(),
        new JPGHandler(),
        new DDSHandler(),
        new PKMHandler(),
        new ASTCHandler(),
#endif
        new T3XHandler()
    };
}

ImageModule::~ImageModule()
{
    for (auto* handler : this->formatHandlers)
        handler->Release();
}

ImageData* ImageModule::NewImageData(Data* data)
{
    return new ImageData(data);
}

ImageData* ImageModule::NewImageData(int width, int height, PixelFormat format)
{
    return new ImageData(width, height, format);
}

ImageData* ImageModule::NewImageData(int width, int height, PixelFormat format, void* data,
                                     bool own)
{
    return new ImageData(width, height, format, data, own);
}

CompressedImageData* ImageModule::NewCompressedData(Data* data)
{
    return new CompressedImageData(this->formatHandlers, data);
}

bool ImageModule::IsCompressed(Data* data)
{
    for (FormatHandler* handler : formatHandlers)
    {
        if (handler->CanParseCompressed(data))
            return true;
    }

    return false;
}

const std::list<FormatHandler*>& ImageModule::GetFormatHandlers() const
{
    return this->formatHandlers;
}

// clang-format off
constexpr auto pixelFormats = BidirectionalMap<>::Create(
    "unknown",         PIXELFORMAT_UNKNOWN,

    "normal",          PIXELFORMAT_NORMAL,
    "r8",              PIXELFORMAT_R8_UNORM,
    "rgb565",          PIXELFORMAT_RGB565_UNORM,
    "rgba8",           PIXELFORMAT_RGBA8_UNORM,
    "rgba16",          PIXELFORMAT_RGBA16_UNORM,
    "stencil8",        PIXELFORMAT_STENCIL8,
    "depth16",         PIXELFORMAT_DEPTH16_UNORM,
    "depth24",         PIXELFORMAT_DEPTH24_UNORM,
    "depth32f",        PIXELFORMAT_DEPTH32_FLOAT,
    "depth24stencil8", PIXELFORMAT_DEPTH24_UNORM_STENCIL8,

    "DXT1",            PIXELFORMAT_DXT1_UNORM,
    "DXT3",            PIXELFORMAT_DXT3_UNORM,
    "DXT5",            PIXELFORMAT_DXT5_UNORM,
    "BC4",             PIXELFORMAT_BC4_UNORM,
    "BC4s",            PIXELFORMAT_BC4_SNORM,
    "BC5",             PIXELFORMAT_BC5_UNORM,
    "BC5s",            PIXELFORMAT_BC5_SNORM,
    "BC6h",            PIXELFORMAT_BC6H_UFLOAT,
    "BC6hs",           PIXELFORMAT_BC6H_FLOAT,
    "BC7",             PIXELFORMAT_BC7_UNORM,
    "PVR1rgb2",        PIXELFORMAT_PVR1_RGB2_UNORM,
    "PVR1rgb4",        PIXELFORMAT_PVR1_RGB4_UNORM,
    "PVR1rgba2",       PIXELFORMAT_PVR1_RGBA2_UNORM,
    "PVR1rgba4",       PIXELFORMAT_PVR1_RGBA4_UNORM,
    "ETC1",            PIXELFORMAT_ETC1_UNORM,
    "ETC2rgb",         PIXELFORMAT_ETC2_RGB_UNORM,
    "ETC2rgba",        PIXELFORMAT_ETC2_RGBA_UNORM,
    "ETC2rgba1",       PIXELFORMAT_ETC2_RGBA1_UNORM,
    "EACr",            PIXELFORMAT_EAC_R_UNORM,
    "EACrs",           PIXELFORMAT_EAC_R_SNORM,
    "EACrg",           PIXELFORMAT_EAC_RG_UNORM,
    "EACrgs",          PIXELFORMAT_EAC_RG_SNORM,
    "ASTC4x4",         PIXELFORMAT_ASTC_4x4,
    "ASTC5x4",         PIXELFORMAT_ASTC_5x4,
    "ASTC5x5",         PIXELFORMAT_ASTC_5x5,
    "ASTC6x5",         PIXELFORMAT_ASTC_6x5,
    "ASTC6x6",         PIXELFORMAT_ASTC_6x6,
    "ASTC8x5",         PIXELFORMAT_ASTC_8x5,
    "ASTC8x6",         PIXELFORMAT_ASTC_8x6,
    "ASTC8x8",         PIXELFORMAT_ASTC_8x8,
    "ASTC10x5",        PIXELFORMAT_ASTC_10x5,
    "ASTC10x6",        PIXELFORMAT_ASTC_10x6,
    "ASTC10x8",        PIXELFORMAT_ASTC_10x8,
    "ASTC10x10",       PIXELFORMAT_ASTC_10x10,
    "ASTC12x10",       PIXELFORMAT_ASTC_12x10,
    "ASTC12x12",       PIXELFORMAT_ASTC_12x12
);
// clang-format on

bool ImageModule::GetConstant(const char* in, PixelFormat& out)
{
    return pixelFormats.Find(in, out);
}

bool ImageModule::GetConstant(PixelFormat in, const char*& out)
{
    return pixelFormats.ReverseFind(in, out);
}
