#include <utilities/decoder/decoder.hpp>

#include <common/exception.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

using namespace love;

Type Decoder::type("Decoder", &Object::type);

Decoder::Decoder(Stream* stream, int bufferSize) :
    stream(stream),
    bufferSize(bufferSize),
    sampleRate(DEFAULT_SAMPLE_RATE),
    buffer(nullptr),
    eof(false)
{
    if (!stream->IsReadable() || !stream->IsSeekable())
        throw love::Exception("Decoder input stream must be readable and seekable.");

    try
    {
        this->buffer = std::make_unique<uint8_t[]>(bufferSize);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }
}

void* Decoder::GetBuffer() const
{
    return this->buffer.get();
}

int Decoder::GetSampleRate() const
{
    return this->sampleRate;
}

int Decoder::GetSize() const
{
    return this->bufferSize;
}

bool Decoder::IsFinished()
{
    return this->eof;
}

// clang-format off
constexpr BidirectionalMap streamSources = {
    "memory", Decoder::STREAM_MEMORY,
    "file",   Decoder::STREAM_FILE
};
// clang-format on

bool Decoder::GetConstant(const char* in, Decoder::StreamSource& out)
{
    return streamSources.Find(in, out);
}

bool Decoder::GetConstant(Decoder::StreamSource in, const char*& out)
{
    return streamSources.ReverseFind(in, out);
}

SmallTrivialVector<const char*, 2> Decoder::GetConstants(Decoder::StreamSource)
{
    return streamSources.GetNames();
}
