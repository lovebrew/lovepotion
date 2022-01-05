#include "objects/image/image.h"
#include "objects/texture/texture.h"

using namespace love;

Image::Slices::Slices(TextureType type) : textureType(type)
{}

void Image::Slices::Clear()
{
    this->data.clear();
}

void Image::Slices::Set(int slice, int mipmap, ImageDataBase* data)
{
    if (slice >= (int)this->data.size())
        this->data.resize(slice + 1);

    if (mipmap >= (int)this->data[slice].size())
        this->data[slice].resize(mipmap + 1);

    this->data[slice][mipmap].Set(data);
}

ImageDataBase* Image::Slices::Get(int slice, int mipmap) const
{
    if (slice < 0 || slice >= this->GetSliceCount(mipmap))
        return nullptr;

    if (mipmap < 0 || mipmap >= this->GetMipmapCount(slice))
        return nullptr;

    return data[slice][mipmap].Get();
}

void Image::Slices::Add(CompressedImageData* data, int startslice, int startmip, bool addallslices,
                        bool addallmips)
{
    int sliceCount = addallslices ? data->GetSliceCount() : 1;
    int mipCount   = addallmips ? data->GetMipmapCount() : 1;

    for (int mip = 0; mip < mipCount; mip++)
    {
        for (int slice = 0; slice < sliceCount; slice++)
            this->Set(startslice + slice, startmip + mip, data->GetSlice(slice, mip));
    }
}

int Image::Slices::GetSliceCount(int mip) const
{
    return (int)data.size();
}

int Image::Slices::GetMipmapCount(int slice) const
{
    if (slice < 0 || slice >= (int)data.size())
        return 0;

    return data[slice].size();
}

Image::MipmapsType Image::Slices::Validate() const
{
    int slicecount = this->GetSliceCount();
    int mipcount   = this->GetMipmapCount(0);

    if (slicecount == 0 || mipcount == 0)
        throw love::Exception("At least one ImageData or CompressedImageData is required!");

    ImageDataBase* firstdata = this->Get(0, 0);

    int w     = firstdata->GetWidth();
    int h     = firstdata->GetHeight();
    int depth = 1;

    PixelFormat format = firstdata->GetFormat();

    int expectedmips = Texture::GetTotalMipmapCount(w, h, depth);

    if (mipcount != expectedmips && mipcount != 1)
        throw love::Exception(
            "Image does not have all required mipmap levels (expected %d, got %d)", expectedmips,
            mipcount);

    int mipw = w;
    int miph = h;
    // int mipslices = slicecount;

    for (int mip = 0; mip < mipcount; mip++)
    {
        for (int slice = 0; slice < slicecount; slice++)
        {
            auto slicedata = this->Get(slice, mip);

            if (slicedata == nullptr)
                throw love::Exception("Missing image data (slice %d, mipmap level %d)", slice + 1,
                                      mip + 1);

            int realw = slicedata->GetWidth();
            int realh = slicedata->GetHeight();

            if (this->GetMipmapCount(slice) != mipcount)
                throw love::Exception("All Image layers must have the same mipmap count.");

            if (mipw != realw)
                throw love::Exception("Width of image data (slice %d, mipmap level %d) is "
                                      "incorrect (expected %d, got %d)",
                                      slice + 1, mip + 1, mipw, realw);

            if (miph != realh)
                throw love::Exception("Height of image data (slice %d, mipmap level %d) is "
                                      "incorrect (expected %d, got %d)",
                                      slice + 1, mip + 1, miph, realh);

            if (format != slicedata->GetFormat())
                throw love::Exception(
                    "All Image slices and mipmaps must have the same pixel format.");
        }

        mipw = std::max(mipw / 2, 1);
        miph = std::max(miph / 2, 1);
    }

    if (mipcount > 1)
        return MIPMAPS_DATA;
    else
        return MIPMAPS_NONE;
}
