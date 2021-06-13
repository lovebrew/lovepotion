#include "modules/sound/sound.h"

#include <vector>

using namespace love;

love::Type Sound::type("Sound", &Module::type);

struct DecoderImpl
{
    Decoder* (*Create)(FileData* data, int bufferSize);
    bool (*Accepts)(const std::string& ext);
};

template<typename DecoderType>
DecoderImpl DecoderImplFor()
{
    DecoderImpl decoderImpl;

    decoderImpl.Create = [](FileData* data, int bufferSize) -> Decoder* {
        return new DecoderType(data, bufferSize);
    };

    decoderImpl.Accepts = [](const std::string& ext) -> bool { return DecoderType::Accepts(ext); };

    return decoderImpl;
}

Sound::~Sound()
{
    MP3Decoder::Quit();
}

Decoder* Sound::NewDecoder(FileData* data, int bufferSize)
{
    std::string ext = data->GetExtension();
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    std::vector<DecoderImpl> possibilities = { DecoderImplFor<VorbisDecoder>(),
                                               DecoderImplFor<MP3Decoder>(),
                                               DecoderImplFor<WaveDecoder>(),
                                               DecoderImplFor<FLACDecoder>(),
                                               DecoderImplFor<ModPlugDecoder>() };

    for (DecoderImpl& item : possibilities)
    {
        if (item.Accepts(ext))
            return item.Create(data, bufferSize);
    }

    /* extension detection fails, let's probe 'em */
    std::string decodingErrors = "Failed to determine file type:\n";

    for (DecoderImpl& item : possibilities)
    {
        try
        {
            Decoder* decoder = item.Create(data, bufferSize);

            return decoder;
        }
        catch (love::Exception& e)
        {
            decodingErrors += e.what() + '\n';
        }
    }

    /* probing failure, throw the errors */
    throw love::Exception(decodingErrors.c_str());

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
