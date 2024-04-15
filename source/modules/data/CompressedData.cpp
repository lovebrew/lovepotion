#include "modules/data/CompressedData.hpp"

#include "common/Exception.hpp"

#include <cstring>

namespace love
{
    Type CompressedData::type("CompressedData", &Data::type);

    CompressedData::CompressedData(Compressor::Format format, char* data, size_t size,
                                   size_t rawSize, bool own) :
        format(format),
        data(nullptr),
        dataSize(size),
        originalSize(rawSize)
    {
        if (own)
            this->data = data;
        else
        {
            try
            {
                this->data = new char[this->dataSize];
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception(E_OUT_OF_MEMORY);
            }

            std::memcpy(this->data, data, this->dataSize);
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
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        std::memcpy(this->data, other.data, this->dataSize);
    }

    CompressedData::~CompressedData()
    {
        delete[] this->data;
    }

    CompressedData* CompressedData::clone() const
    {
        return new CompressedData(*this);
    }

    Compressor::Format CompressedData::getFormat() const
    {
        return this->format;
    }

    size_t CompressedData::getDecompressedSize() const
    {
        return this->originalSize;
    }

    void* CompressedData::getData() const
    {
        return this->data;
    }

    size_t CompressedData::getSize() const
    {
        return this->dataSize;
    }
} // namespace love
