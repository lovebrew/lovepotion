#include "objects/datastream/datastream.h"

#include "common/data.h"
#include "common/exception.h"

#include <algorithm>

using namespace love;

love::Type DataStream::type("DataStream", &Stream::type);

DataStream::DataStream(Data* data) :
    data(data),
    memory((const uint8_t*)data->GetData()),
    writableMemory((uint8_t*)data->GetData()),
    offset(0),
    size(data->GetSize())
{}

DataStream::DataStream(const DataStream& other) :
    data(other.data),
    memory((const uint8_t*)data->GetData()),
    writableMemory((uint8_t*)data->GetData()),
    offset(0),
    size(other.size)
{}

DataStream* DataStream::Clone()
{
    return new DataStream(*this);
}

DataStream::~DataStream()
{}

bool DataStream::IsReadable() const
{
    return true;
}

bool DataStream::IsWritable() const
{
    return this->writableMemory != nullptr;
}

bool DataStream::IsSeekable() const
{
    return true;
}

int64_t DataStream::Read(void* data, int64_t size)
{
    if (size <= 0)
        return 0;

    if (this->offset >= this->GetSize())
        return 0;

    int64_t readSize = std::min<int64_t>(size, this->GetSize() - this->offset);
    memcpy(this->data, this->memory + this->offset, readSize);

    this->offset += readSize;

    return readSize;
}

bool DataStream::Write(const void* data, int64_t size)
{
    if (size <= 0 || this->writableMemory == nullptr)
        return false;

    if (this->offset >= this->GetSize())
        return false;

    int64_t writeSize = std::min<int64_t>(size, this->GetSize() - offset);
    memcpy(this->writableMemory + this->offset, this->data, writeSize);

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
    if (origin == SEEK_ORIGIN_CURRENT)
        position += this->offset;
    else if (origin == SEEK_ORIGIN_END)
        position += this->size;

    if (position < 0 || position > this->size)
        return false;

    this->offset = position;

    return true;
}

int64_t DataStream::Tell()
{
    return this->offset;
}
