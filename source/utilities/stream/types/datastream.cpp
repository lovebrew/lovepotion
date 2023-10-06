#include <common/exception.hpp>

#include <common/data.hpp>
#include <utilities/stream/types/datastream.hpp>

#include <algorithm>

using namespace love;

Type DataStream::type("DataStream", &Stream::type);

DataStream::DataStream(Data* data) :
    data(data),
    memory((const uint8_t*)data->GetData()),
    writeable((uint8_t*)data->GetData()),
    offset(0),
    size(data->GetSize())
{}

DataStream::DataStream(const DataStream& other) :
    data(other.data),
    memory(other.memory),
    writeable(other.writeable),
    offset(0),
    size(other.size)
{}

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
    return this->writeable != nullptr;
}

bool DataStream::IsSeekable() const
{
    return true;
}

int64_t DataStream::Read(void* data, int64_t size)
{
    if (size <= 0)
        return 0;

    if ((int64_t)this->offset >= this->GetSize())
        return 0;

    auto readSize = std::min<int64_t>(size, this->GetSize() - this->offset);
    std::memcpy(data, this->memory + this->offset, readSize);

    this->offset += readSize;
    return readSize;
}

bool DataStream::Write(const void* data, int64_t size)
{
    if (size <= 0 || this->writeable == nullptr)
        return false;

    if ((int64_t)this->offset >= this->size)
        return false;

    auto writeSize = std::min<int64_t>(size, this->GetSize() - this->offset);
    std::memcpy(this->writeable + this->offset, data, writeSize);

    this->offset += writeSize;
    return true;
}

bool DataStream::Flush()
{
    return true;
}

int64_t DataStream::GetSize()
{
    return this->size;
}

bool DataStream::Seek(int64_t position, SeekOrigin origin)
{
    if (origin == SeekOrigin::ORIGIN_CURRENT)
        position += this->offset;
    else if (origin == SeekOrigin::ORIGIN_END)
        position += this->size;

    if (position < 0 || position > (int64_t)this->size)
        return false;

    this->offset = position;
    return true;
}

int64_t DataStream::Tell()
{
    return this->offset;
}
