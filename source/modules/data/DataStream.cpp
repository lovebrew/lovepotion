#include "common/Data.hpp"
#include "common/Exception.hpp"
#include "common/int.hpp"

#include "modules/data/DataStream.hpp"

#include <algorithm>

#include <cstring>

namespace love
{
    Type DataStream::type("DataStream", &Stream::type);

    DataStream::DataStream(Data* data) :
        data(data),
        memory((const uint8_t*)data->getData()),
        writableMemory((uint8_t*)data->getData()),
        offset(0),
        size(data->getSize())
    {}

    DataStream::DataStream(const DataStream& other) :
        data(other.data),
        memory(other.memory),
        writableMemory(other.writableMemory),
        offset(0),
        size(other.size)
    {}

    DataStream::~DataStream()
    {}

    DataStream* DataStream::clone() const
    {
        return new DataStream(*this);
    }

    bool DataStream::isReadable() const
    {
        return true;
    }

    bool DataStream::isWritable() const
    {
        return this->writableMemory != nullptr;
    }

    bool DataStream::isSeekable() const
    {
        return true;
    }

    int64_t DataStream::read(void* data, int64_t size)
    {
        if (size <= 0)
            return 0;

        if ((int64_t)this->offset >= this->size)
            return 0;

        auto readSize = std::min<int64_t>(size, this->getSize() - this->offset);
        std::memcpy(data, this->memory + this->offset, readSize);

        this->offset += readSize;
        return readSize;
    }

    bool DataStream::write(const void* data, int64_t size)
    {
        if (size <= 0 || this->writableMemory == nullptr)
            return false;

        if ((int64_t)this->offset > this->size)
            return false;

        auto writeSize = std::min<int64_t>(size, this->getSize() - this->offset);
        std::memcpy(this->writableMemory + this->offset, data, writeSize);

        this->offset += writeSize;
        return true;
    }

    bool DataStream::flush()
    {
        return true;
    }

    int64_t DataStream::getSize()
    {
        return this->size;
    }

    bool DataStream::seek(int64_t position, SeekOrigin origin)
    {
        if (origin == SEEKORIGIN_CURRENT)
            position += this->offset;
        else if (origin == SEEKORIGIN_END)
            position += this->size;

        if (position < 0 || position > (int64_t)this->size)
            return false;

        this->offset = position;
        return true;
    }

    int64_t DataStream::tell()
    {
        return this->offset;
    }
} // namespace love
