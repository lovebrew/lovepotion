#include <common/data.hpp>

#include <modules/sound/sound.hpp>

#include <utilities/decoder/types/flacdecoder.hpp>
#include <utilities/decoder/types/modplugdecoder.hpp>
#include <utilities/decoder/types/mp3decoder.hpp>
#include <utilities/decoder/types/vorbisdecoder.hpp>
#include <utilities/decoder/types/wavedecoder.hpp>

using namespace love;

struct DecoderImpl
{
    Decoder* (*Create)(Stream* stream, int bufferSize);
};

template<typename DecoderType>
DecoderImpl DecoderImplFor()
{
    DecoderImpl decoderImpl;

    decoderImpl.Create = [](Stream* stream, int bufferSize) -> Decoder* {
        return new DecoderType(stream, bufferSize);
    };

    return decoderImpl;
}

Sound::~Sound()
{}

Decoder* Sound::NewDecoder(Stream* stream, int bufferSize)
{
    // clang-format off
    std::vector<DecoderImpl> possibleDecoders =
    {
        DecoderImplFor<WaveDecoder>(),
        DecoderImplFor<VorbisDecoder>(),
        DecoderImplFor<MP3Decoder>(),
        DecoderImplFor<FLACDecoder>(),
        DecoderImplFor<ModPlugDecoder>()
    };
    // clang-format on

    std::string errors;

    for (auto& possibleDecoder : possibleDecoders)
    {
        try
        {
            stream->Seek(0);
            auto* decoder = possibleDecoder.Create(stream, bufferSize);

            return decoder;
        }
        catch (love::Exception& e)
        {
            errors += std::string(e.what()) + "\n";
        }
    }

    /* probing failure, throw the errors */
    throw love::Exception("No suitable audio decoders found.\n%s", errors.c_str());

    return nullptr;
}

SoundData* Sound::NewSoundData(Decoder* decoder)
{
    return new SoundData(decoder);
}

SoundData* Sound::NewSoundData(int samples, int sampleRate, int bitDepth, int channels)
{
    return new SoundData(samples, sampleRate, bitDepth, channels);
}

SoundData* Sound::NewSoundData(void* data, int samples, int sampleRate, int bitDepth, int channels)
{
    return new SoundData(data, samples, sampleRate, bitDepth, channels);
}
