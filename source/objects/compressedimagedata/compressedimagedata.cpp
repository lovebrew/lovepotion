#include <objects/compressedimagedata/compressedimagedata.hpp>

using namespace love;

Type CompressedImageData::type("CompressedImageData", &Data::type);

CompressedImageData::CompressedImageData(const std::list<FormatHandler*>& formats, Data* fileData) :
    format(PIXELFORMAT_UNKNOWN),
    sRGB(false)
{
    FormatHandler* parser = nullptr;

    for (auto* handler : formats)
    {
        if (handler->CanParseCompressed(fileData))
        {
            parser = handler;
            break;
        }
    }

    if (parser == nullptr)
        throw love::Exception("Could not parse compressed data: Unknown format.");

    memory = parser->ParseCompressed(fileData, this->images, format, sRGB);

    if (memory == nullptr)
        throw love::Exception("Could not parse compressed data.");

    if (format == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Could not parse compressed data: Unknown format.");

    if (this->images.size() == 0 || this->memory->GetSize() == 0)
        throw love::Exception("Could not parse compressed data: No valid data?");
}

CompressedImageData::CompressedImageData(const CompressedImageData& data) :
    format(data.format),
    sRGB(data.sRGB)
{
    this->memory.Set(data.memory->Clone(), Acquire::NORETAIN);

    for (const auto& image : data.images)
    {
        auto slice = new CompressedSlice(image->GetFormat(), image->GetWidth(), image->GetHeight(),
                                         memory, image->GetOffset(), image->GetSize());

        this->images.push_back(slice);
        slice->Release();
    }
}

CompressedImageData* CompressedImageData::Clone() const
{
    return new CompressedImageData(*this);
}

size_t CompressedImageData::GetSize() const
{
    return this->memory->GetSize();
}

void* CompressedImageData::GetData() const
{
    return this->memory->GetData();
}

int CompressedImageData::GetMipmapCount() const
{
    return (int)this->images.size();
}

int CompressedImageData::GetSliceCount(int /*mip*/) const
{
    return 1;
}

size_t CompressedImageData::GetSize(int mipmap) const
{
    this->CheckSliceExists(0, mipmap);

    return this->images[mipmap]->GetSize();
}

void* CompressedImageData::GetData(int mipmap) const
{
    this->CheckSliceExists(0, mipmap);

    return this->images[mipmap]->GetData();
}

int CompressedImageData::GetWidth(int mipmap) const
{
    this->CheckSliceExists(0, mipmap);

    return this->images[mipmap]->GetWidth();
}

int CompressedImageData::GetHeight(int mipmap) const
{
    this->CheckSliceExists(0, mipmap);

    return this->images[mipmap]->GetHeight();
}

PixelFormat CompressedImageData::GetFormat() const
{
    return this->format;
}

bool CompressedImageData::IsSRGB() const
{
    return this->sRGB;
}

CompressedSlice* CompressedImageData::GetSlice(int slice, int mipmap) const
{
    this->CheckSliceExists(slice, mipmap);

    return this->images[mipmap].Get();
}

void CompressedImageData::CheckSliceExists(int slice, int mipmap) const
{
    if (slice != 0)
        throw love::Exception("Slice index %d does not exists", slice + 1);

    if (mipmap < 0 || mipmap >= (int)this->images.size())
        throw love::Exception("Mipmap level %d does not exist", mipmap + 1);
}
