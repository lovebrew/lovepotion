#include <common/exception.hpp>

#include <common/data.hpp>
#include <utilities/stream/types/datastream.hpp>

#include <algorithm>

using namespace love;

Type DataStream::type("DataStream", &Stream::type);

DataStream::DataStream(Data* data) : data(data), memory {}
{
    this->memory.readOnly.reset((const uint8_t*)data->GetData());
    this->memory.writeable.reset((uint8_t*)data->GetData());

    this->memory.size = data->GetSize();
}

DataStream::DataStream(const DataStream& other) : data(other.data), memory {}
{
    this->memory.readOnly.reset(other.memory.readOnly.get());
    this->memory.writeable.reset(other.memory.writeable.get());

    this->memory.size = other.memory.size;
}

DataStream* DataStream::Clone()
{
    return new DataStream(*this);
}

bool DataStream::IsReadable() const
{
    return true;
}

bool DataStream::IsWritable() const
{
    return this->memory.writeable != nullptr;
}

bool DataStream::IsSeekable() const
{
    return true;
}

int64_t DataStream::Read(void* data, int64_t size)
{
    if (size <= 0)
        return 0;

    if ((int64_t)this->memory.offset >= this->memory.size)
        return 0;

    auto readSize = std::min<int64_t>(size, this->memory.size - this->memory.offset);
    std::copy_n(this->memory.readOnly.get() + this->memory.offset, readSize, (uint8_t*)data);

    this->memory.offset += readSize;

    return readSize;
}

bool DataStream::Write(const void* data, int64_t size)
{
    if (size <= 0 || !this->memory.writeable)
        return false;

    if ((int64_t)this->memory.offset >= this->memory.size)
        return false;

    auto writeSize = std::min<int64_t>(size, this->memory.size - this->memory.offset);
    std::copy_n((uint8_t*)data, writeSize, this->memory.writeable.get() + this->memory.offset);

    this->memory.offset += writeSize;

    return true;
}

bool DataStream::Flush()
{
    return true;
}

int64_t DataStream::GetSize()
{
    return this->memory.size;
}

bool DataStream::Seek(int64_t position, SeekOrigin origin)
{
    if (origin == SeekOrigin::ORIGIN_CURRENT)
        position += this->memory.offset;
    else if (origin == SeekOrigin::ORIGIN_END)
        position += this->memory.size;

    if (position < 0 || position > (int64_t)this->memory.size)
        return false;

    this->memory.offset = position;

    return true;
}

int64_t DataStream::Tell()
{
    return this->memory.offset;
}
