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
    "rgba8",           PIXELFORMAT_RGBA8,
    "rgba16",          PIXELFORMAT_RGBA16,
    "stencil8",        PIXELFORMAT_STENCIL8,
    "depth16",         PIXELFORMAT_DEPTH16,
    "depth24",         PIXELFORMAT_DEPTH24,
    "depth32f",        PIXELFORMAT_DEPTH32F,
    "depth24stencil8", PIXELFORMAT_DEPTH24_STENCIL8,
    "tex3ds_rgba8",    PIXELFORMAT_TEX3DS_RGBA8,

    "DXT1",            PIXELFORMAT_DXT1,
    "DXT3",            PIXELFORMAT_DXT3,
    "DXT5",            PIXELFORMAT_DXT5,
    "BC4",             PIXELFORMAT_BC4,
    "BC4s",            PIXELFORMAT_BC4s,
    "BC5",             PIXELFORMAT_BC5,
    "BC5s",            PIXELFORMAT_BC5s,
    "BC6h",            PIXELFORMAT_BC6H,
    "BC6hs",           PIXELFORMAT_BC6Hs,
    "BC7",             PIXELFORMAT_BC7,
    "PVR1rgb2",        PIXELFORMAT_PVR1_RGB2,
    "PVR1rgb4",        PIXELFORMAT_PVR1_RGB4,
    "PVR1rgba2",       PIXELFORMAT_PVR1_RGBA2,
    "PVR1rgba4",       PIXELFORMAT_PVR1_RGBA4,
    "ETC1",            PIXELFORMAT_ETC1,
    "ETC2rgb",         PIXELFORMAT_ETC2_RGB,
    "ETC2rgba",        PIXELFORMAT_ETC2_RGBA,
    "ETC2rgba1",       PIXELFORMAT_ETC2_RGBA1,
    "EACr",            PIXELFORMAT_EAC_R,
    "EACrs",           PIXELFORMAT_EAC_Rs,
    "EACrg",           PIXELFORMAT_EAC_RG,
    "EACrgs",          PIXELFORMAT_EAC_RGs,
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
