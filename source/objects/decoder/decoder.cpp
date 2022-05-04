#include "objects/decoder/decoder.h"

#include "common/bidirectionalmap.h"

using namespace love;

love::Type Decoder::type("Decoder", &Object::type);

Decoder::Decoder(Stream* stream, int bufferSize) :
    stream(stream),
    bufferSize(bufferSize),
    sampleRate(DEFAULT_SAMPLE_RATE),
    buffer(0),
    eof(false)
{
    if (!stream->IsReadable() || !stream->IsSeekable())
        throw love::Exception("Decoder input stream must be readable and seekable.");

    this->buffer = new char[bufferSize];
}

Decoder::~Decoder()
{
    if (this->buffer != 0)
        delete[](char*) this->buffer;
}

void* Decoder::GetBuffer() const
{
    return this->buffer;
}

int Decoder::GetSize() const
{
    return this->bufferSize;
}

int Decoder::GetSampleRate() const
{
    return this->sampleRate;
}

bool Decoder::IsFinished()
{
    return this->eof;
}

// clang-format off
constexpr auto streamSources = BidirectionalMap<>::Create(
    "memory", Decoder::STREAM_SOURCE_MEMORY,
    "file",   Decoder::STREAM_SOURCE_FILE
);
// clang-format on

bool Decoder::GetConstant(const char* in, Decoder::StreamSource& out)
{
    return streamSources.Find(in, out);
}

bool Decoder::GetConstant(Decoder::StreamSource in, const char*& out)
{
    return streamSources.ReverseFind(in, out);
}

std::vector<const char*> Decoder::GetConstants(Decoder::StreamSource)
{
    return streamSources.GetNames();
}
