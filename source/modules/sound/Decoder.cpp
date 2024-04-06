#include "modules/sound/Decoder.hpp"
#include "common/Exception.hpp"

namespace love
{
    Type Decoder::type("Decoder", &Object::type);

    Decoder::Decoder(Stream* stream, int bufferSize) :
        stream(stream),
        bufferSize(bufferSize),
        sampleRate(DEFAULT_SAMPLE_RATE),
        buffer(nullptr),
        eof(false)
    {
        if (!stream->isReadable() || !stream->isSeekable())
            throw love::Exception("Decoder input stream must be readable and seekable.");

        try
        {
            this->buffer = new char[bufferSize];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }
    }

    Decoder::~Decoder()
    {
        if (this->buffer != nullptr)
            delete[] (char*)this->buffer;
    }

    void* Decoder::getBuffer() const
    {
        return this->buffer;
    }

    int Decoder::getSize() const
    {
        return this->bufferSize;
    }

    int Decoder::getSampleRate() const
    {
        return this->sampleRate;
    }

    bool Decoder::isFinished()
    {
        return this->eof;
    }
} // namespace love
