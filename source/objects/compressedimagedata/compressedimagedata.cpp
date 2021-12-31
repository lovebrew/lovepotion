#include "objects/compressedimagedata/compressedimagedata.h"
#include "common/exception.h"

using namespace love;

love::Type CompressedImageData::type("CompressedImageData", &Data::type);

CompressedImageData::CompressedImageData(const std::list<FormatHandler*>& formats, Data* fileData) :
    format(PIXELFORMAT_UNKNOWN),
    sRGB(false)
{
    FormatHandler* parser = nullptr;

    for (FormatHandler* handler : formats)
    {
        if (handler->CanParseCompressed(fileData))
        {
            parser = handler;
            break;
        }
    }

    if (parser == nullptr)
        throw love::Exception("Could not parse compressed data: Unknown format.");

    this->memory = parser->ParseCompressed(fileData, images, format, this->sRGB);

    if (this->memory == nullptr)
        throw love::Exception("Could not parse compressed data.");

    if (format == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Could not parse compressed data: Unknown format.");

    if (this->images.size() == 0 || this->memory->size == 0)
        throw love::Exception("Could not parse compressed data: No valid data?");
}

CompressedImageData::CompressedImageData(const CompressedImageData& other) :
    format(other.format),
    sRGB(other.sRGB)
{
    this->memory.Set(new CompressedMemory(other.memory->size), Acquire::NORETAIN);
    memcpy(this->memory->data, other.memory->data, this->memory->size);

    for (const auto& image : other.images)
    {
        auto slice = new CompressedSlice(image->GetFormat(), image->GetWidth(), image->GetHeight(),
                                         memory, image->GetOffset(), image->GetSize());

        images.push_back(slice);
        slice->Release();
    }
}

CompressedImageData* CompressedImageData::Clone() const
{
    return new CompressedImageData(*this);
}

CompressedImageData::~CompressedImageData()
{}

size_t CompressedImageData::GetSize() const
{
    return this->memory->size;
}

void* CompressedImageData::GetData() const
{
    return this->memory->data;
}

int CompressedImageData::GetMipmapCount() const
{
    return (int)this->images.size();
}

int CompressedImageData::GetSliceCount(int /*mip*/) const
{
    return 1;
}

size_t CompressedImageData::GetSize(int mipLevel) const
{
    this->CheckSliceExists(0, mipLevel);

    return this->images[mipLevel]->GetSize();
}

void* CompressedImageData::GetData(int mipLevel) const
{
    this->CheckSliceExists(0, mipLevel);

    return this->images[mipLevel]->GetData();
}

int CompressedImageData::GetWidth(int mipLevel) const
{
    this->CheckSliceExists(0, mipLevel);

    return this->images[mipLevel]->GetWidth();
}

int CompressedImageData::GetHeight(int mipLevel) const
{
    this->CheckSliceExists(0, mipLevel);

    return this->images[mipLevel]->GetHeight();
}

PixelFormat CompressedImageData::GetFormat() const
{
    return this->format;
}

bool CompressedImageData::IsSRGB() const
{
    return this->sRGB;
}

CompressedSlice* CompressedImageData::GetSlice(int slice, int mipLevel) const
{
    this->CheckSliceExists(slice, mipLevel);

    return this->images[mipLevel].Get();
}

void CompressedImageData::CheckSliceExists(int slice, int mipLevel) const
{
    if (slice != 0)
        throw love::Exception("Slice index %d does not exist.", slice + 1);

    if (mipLevel < 0 || mipLevel >= (int)this->images.size())
        throw love::Exception("Mipmap level %d does not exist.", mipLevel + 1);
}
