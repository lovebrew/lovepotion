#include <common/data.hpp>
#include <common/exception.hpp>
#include <common/strongreference.hpp>

#include <objects/data/bytedata/bytedata.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/stream/stream.hpp>

#include <algorithm>
#include <limits>

using namespace love;

Type Stream::type("Stream", &Object::type);

Data* Stream::Read(int64_t size)
{
    int64_t max     = std::numeric_limits<int64_t>::max();
    int64_t current = 0;

    if (this->IsSeekable())
    {
        max     = this->GetSize();
        current = this->Tell();
    }

    current = std::clamp(current, (int64_t)0, max);

    if (current + size > max)
        size = max - current;

    StrongReference<ByteData> destination(new ByteData(size, false), Acquire::NORETAIN);

    int64_t bytesRead = this->Read(destination->GetData(), size);

    if (bytesRead < 0 || (bytesRead == 0 && bytesRead != size))
        throw love::Exception("Could not read from stream.");

    if (bytesRead < size)
        destination.Set(new ByteData(destination->GetData(), (size_t)bytesRead), Acquire::NORETAIN);

    destination->Retain();

    return destination;
}

bool Stream::Write(Data* source)
{
    return this->Write(source, 0, source->GetSize());
}

bool Stream::Write(Data* source, int64_t offset, int64_t size)
{
    if (offset < 0 || size < 0 || offset + size > (int64_t)source->GetSize())
        throw love::Exception(
            "Offset and size parameters do not fit within the given Data's size.");

    return this->Write((const uint8_t*)source->GetData() + offset, size);
}
