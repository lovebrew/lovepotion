#include "common/Exception.hpp"

#include "modules/data/ByteData.hpp"

#include <algorithm>
#include <cstring>

namespace love
{
    Type ByteData::type("ByteData", &Data::type);

    ByteData::ByteData(size_t size, bool clear) : size(size)
    {
        this->create();

        if (clear)
            std::fill_n(this->data, size, 0);
    }

    ByteData::ByteData(const void* data, size_t size) : size(size)
    {
        this->create();

        if (data != nullptr)
            std::memcpy(this->data, data, size);
    }

    ByteData::ByteData(void* data, size_t size, bool owned) : size(size)
    {
        if (owned)
            this->data = (char*)data;
        else
        {
            this->create();

            if (data != nullptr)
                std::memcpy(this->data, data, size);
        }
    }

    ByteData::ByteData(const ByteData& other) : size(other.size)
    {
        this->create();
        std::memcpy(this->data, other.data, this->size);
    }

    ByteData::~ByteData()
    {
        delete[] this->data;
    }

    void ByteData::create()
    {
        if (this->size == 0)
            throw love::Exception(E_DATA_SIZE_MUST_BE_POSITIVE);

        try
        {
            this->data = new char[this->size];
        }
        catch (std::bad_alloc& e)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }
    }

    ByteData* ByteData::clone() const
    {
        return new ByteData(*this);
    }

    void* ByteData::getData() const
    {
        return this->data;
    }

    size_t ByteData::getSize() const
    {
        return this->size;
    }
} // namespace love
