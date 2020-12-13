#include "common/runtime.h"
#include "modules/sound/sound.h"

using namespace love;

love::Type Sound::type("Sound", &Module::type);

struct DecoderImpl
{
    Decoder *(*Create)(FileData * data, int bufferSize);
    bool (*Accepts)(const std::string & ext);
};

template<typename DecoderType>
DecoderImpl DecoderImplFor()
{
    DecoderImpl decoderImpl;

    decoderImpl.Create = [](FileData * data, int bufferSize) -> Decoder *
    {
        return new DecoderType(data, bufferSize);
    };

    decoderImpl.Accepts = [](const std::string & ext) -> bool
    {
        return DecoderType::Accepts(ext);
    };

    return decoderImpl;
}

Sound::~Sound()
{}

Decoder * Sound::NewDecoder(FileData * data, int bufferSize)
{
    std::string ext = data->GetExtension();
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    std::vector<DecoderImpl> possibilities =
    {
        DecoderImplFor<VorbisDecoder>(),
        DecoderImplFor<MP3Decoder>(),
        DecoderImplFor<WaveDecoder>(),
        DecoderImplFor<FLACDecoder>()
    };

    for (DecoderImpl & item : possibilities)
    {
        if (item.Accepts(ext))
            return item.Create(data, bufferSize);
    }

    throw love::Exception("Failed to determine file type.");

    return nullptr;
}

SoundData * Sound::NewSoundData(Decoder * decoder)
{
    return new SoundData(decoder);
}

SoundData * Sound::NewSoundData(int samples, int sampleRate, int bitDepth, int channels)
{
    return new SoundData(samples, sampleRate, bitDepth, channels);
}

SoundData * Sound::NewSoundData(void * data, int samples, int sampleRate, int bitDepth, int channels)
{
    return new SoundData(data, samples, sampleRate, bitDepth, channels);
}
