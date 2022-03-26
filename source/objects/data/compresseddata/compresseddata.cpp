#include "objects/data/compressed/compresseddata.h"

using namespace love;

love::Type CompressedData::type("CompressedData", &Data::type);

CompressedData::CompressedData(Compressor::Format format, char* cdata, size_t compressedSize,
                               size_t rawSize, bool own) :
    format(format),
    data(nullptr),
    dataSize(compressedSize),
    originalSize(rawSize)
{
    if (own)
        this->data = cdata;
    else
    {
        try
        {
            this->data = new char[this->dataSize];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception("Out of memory.");
        }

        memcpy(this->data, cdata, this->dataSize);
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
        this->data = new char[this->dataSize];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    memcpy(this->data, other.data, this->dataSize);
}

CompressedData::~CompressedData()
{
    delete[] this->data;
}

CompressedData* CompressedData::Clone() const
{
    return new CompressedData(*this);
}

Compressor::Format CompressedData::GetFormat() const
{
    return this->format;
}

size_t CompressedData::GetDecompressedSize() const
{
    return this->originalSize;
}

void* CompressedData::GetData() const
{
    return this->data;
}

size_t CompressedData::GetSize() const
{
    return this->dataSize;
}
