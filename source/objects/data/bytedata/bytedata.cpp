#include "objects/data/byte/bytedata.h"

#include <cstring>

using namespace love;

love::Type ByteData::type("ByteData", &Data::type);

ByteData::ByteData(size_t size) : size(size)
{
    this->Create();
    memset(this->data, 0, size);
}

ByteData::ByteData(const void* data, size_t size) : size(size)
{
    this->Create();
    memcpy(this->data, data, size);
}

ByteData::ByteData(void* data, size_t size, bool own) : size(size)
{
    if (own)
        this->data = (char*)data;
    else
    {
        this->Create();
        memcpy(this->data, data, size);
    }
}

ByteData::ByteData(const ByteData& other) : size(other.size)
{
    this->Create();
    memcpy(this->data, other.data, this->size);
}

void ByteData::Create()
{
    if (this->size == 0)
        throw love::Exception("ByteData size must be greater than zero.");

    try
    {
        this->data = new char[this->size];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }
}

ByteData* ByteData::Clone() const
{
    return new ByteData(*this);
}

void* ByteData::GetData() const
{
    return this->data;
}

size_t ByteData::GetSize() const
{
    return this->size;
}

ByteData::~ByteData()
{
    delete[] this->data;
}
