#include "objects/data/compresseddata/compresseddata.hpp"

#include <algorithm>

using namespace love;

Type CompressedData::type("CompressedData", &Data::type);

CompressedData::CompressedData(Compressor::Format format, char* compressedData,
                               size_t compressedSize, size_t rawSize, bool own) :
    format(format),
    data(nullptr),
    dataSize(compressedSize),
    originalSize(rawSize)
{
    if (own)
        this->data.reset(compressedData);
    else
    {
        try
        {
            this->data = std::make_unique<char[]>(dataSize);
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception("Out of memory.");
        }

        std::copy_n(compressedData, this->dataSize, this->data.get());
    }
}

CompressedData::CompressedData(const CompressedData& other) :
    format(other.format),
    data(nullptr),
    dataSize(other.dataSize),
    originalSize(other.originalSize)
{
    try
    {
        this->data = std::make_unique<char[]>(this->dataSize);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    std::copy_n(other.data.get(), this->dataSize, this->data.get());
}

CompressedData* CompressedData::Clone() const
{
    return new CompressedData(*this);
}

void* CompressedData::GetData() const
{
    return this->data.get();
}

size_t CompressedData::GetSize() const
{
    return this->dataSize;
}

Compressor::Format CompressedData::GetFormat() const
{
    return this->format;
}

size_t CompressedData::GetDecompressedSize() const
{
    return this->originalSize;
}
