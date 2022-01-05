#include "objects/image/image.h"
#include "modules/image/imagemodule.h"

#include "deko3d/deko.h"

#include "modules/graphics/graphics.h"

using namespace love;

Image::Image(const Slices& slices, bool validate) :
    Texture(slices.GetTextureType()),
    data(slices),
    mipmapsType(MIPMAPS_NONE),
    sRGB(Graphics::IsGammaCorrect())
{
    if (validate && data.Validate() == MIPMAPS_DATA)
        this->mipmapsType = MIPMAPS_DATA;
}

Image::Image(TextureType type, PixelFormat format, int width, int height, int slices) :
    Image(Slices(textureType), false)
{
    if (love::IsPixelFormatCompressed(format))
        throw love::Exception("This constructor only supports non-compressesd pixel formats.");

    this->Init(format, width, height);
}

Image::Image(const Slices& slices) : Image(slices, true)
{
    ImageDataBase* slice = slices.Get(0, 0);
    this->Init(slice->GetFormat(), slice->GetWidth(), slice->GetHeight());
}

Image::~Image()
{}

void Image::ReplacePixels(const void* data, size_t size, const Rect& rect)
{
    this->texture.replacePixels(::deko3d::Instance().GetData(), ::deko3d::Instance().GetDevice(),
                                data, size, ::deko3d::Instance().GetTextureQueue(), rect);
}

void Image::Init(PixelFormat pixelFormat, int width, int height)
{
    PixelFormat format = pixelFormat;

    if (this->data.Get(0, 0))
    {
        bool success = this->texture.load(format, this->sRGB, this->data.Get(0, 0)->GetData(),
                                          this->data.Get(0, 0)->GetSize(), width, height);

        if (!success)
        {
            const char* formatName = nullptr;
            ImageModule::GetConstant(format, formatName);

            throw love::Exception("Failed to upload image data: format %s not supported",
                                  formatName);
        }
    }
    else
    {
        this->texture.load(pixelFormat, this->sRGB, nullptr, 0, width, height, true);
    }

    this->width  = width;
    this->height = height;

    this->handle = ::deko3d::Instance().RegisterResHandle(this->texture.getDescriptor());

    this->InitQuad();

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}
