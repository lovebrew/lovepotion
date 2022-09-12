#include "objects/data/bytedata/bytedata.hpp"
#include "common/exception.hpp"

#include <algorithm>
#include <cstring>

using namespace love;

Type ByteData::type("ByteData", &Data::type);

ByteData::ByteData(size_t size, bool clear) : size(size)
{
    this->Create();

    if (clear)
        std::fill_n(this->data.get(), this->size, 0);
}

ByteData::ByteData(const void* data, size_t size) : size(size)
{
    this->Create();

    if (data != nullptr)
        std::copy_n((char*)data, size, this->data.get());
}

ByteData::ByteData(void* data, size_t size, bool own) : size(size)
{
    if (own)
        this->data.reset((char*)data);
    else
    {
        this->Create();

        if (data != nullptr)
            std::copy_n((char*)data, size, this->data.get());
    }
}

ByteData::ByteData(const ByteData& other) : size(other.size)
{
    this->Create();
    std::copy_n(other.data.get(), this->size, this->data.get());
}

void ByteData::Create()
{
    if (this->size == 0)
        throw love::Exception("ByteData size must be greater than zero.");

    try
    {
        this->data = std::make_unique<char[]>(this->size);
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
    return this->data.get();
}

size_t ByteData::GetSize() const
{
    return this->size;
}
