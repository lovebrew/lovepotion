#include "objects/decoder/decoder.h"

using namespace love;

love::Type Decoder::type("Decoder", &Object::type);

Decoder::Decoder(Data* data, int bufferSize) :
    data(data),
    bufferSize(bufferSize),
    sampleRate(DEFAULT_SAMPLE_RATE),
    buffer(0),
    eof(false)
{
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
