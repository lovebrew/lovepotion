#include "modules/sound/sound.h"

#include "common/data.h"

#include <vector>

using namespace love;

love::Type Sound::type("Sound", &Module::type);

struct DecoderImpl
{
    Decoder* (*Create)(Stream* stream, int bufferSize);
    bool (*Accepts)(const std::string& ext);
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
{
    MP3Decoder::Quit();
}

Decoder* Sound::NewDecoder(Stream* stream, int bufferSize)
{
    std::vector<DecoderImpl> possibilities = { DecoderImplFor<MP3Decoder>(),
                                               DecoderImplFor<VorbisDecoder>(),
                                               DecoderImplFor<WaveDecoder>(),
                                               DecoderImplFor<FLACDecoder>(),
                                               DecoderImplFor<ModPlugDecoder>() };

    /* extension detection fails, let's probe 'em */
    std::string decodingErrors = "Failed to determine file type:\n";

    for (DecoderImpl& item : possibilities)
    {
        try
        {
            stream->Seek(0);
            Decoder* decoder = item.Create(stream, bufferSize);

            return decoder;
        }
        catch (love::Exception& e)
        {
            decodingErrors += e.what() + '\n';
        }
    }

    /* probing failure, throw the errors */
    throw love::Exception("No suitable audio decoders found. %s", decodingErrors.c_str());

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
