#include "modules/sound/sound.h"

#include "common/data.h"

#include <algorithm>
#include <vector>

using namespace love;

love::Type Sound::type("Sound", &Module::type);

struct DecoderImpl
{
    Decoder* (*Create)(Stream* stream, int bufferSize);
    int (*Probe)(Stream* stream);
    int probeScore;
};

static bool compareProbeScore(const DecoderImpl& a, const DecoderImpl& b)
{
    return a.probeScore > b.probeScore;
}

template<typename DecoderType>
DecoderImpl DecoderImplFor()
{
    DecoderImpl decoderImpl;

    decoderImpl.Create = [](Stream* stream, int bufferSize) -> Decoder* {
        return new DecoderType(stream, bufferSize);
    };

    decoderImpl.Probe = [](Stream* stream) { return DecoderType::Probe(stream); };

    decoderImpl.probeScore = 0;

    return decoderImpl;
}

Sound::~Sound()
{
    MP3Decoder::Quit();
}

Decoder* Sound::NewDecoder(Stream* stream, int bufferSize)
{
    // clang-format off
    std::vector<DecoderImpl> active, possible =
    {
        DecoderImplFor<WaveDecoder>(),
        DecoderImplFor<FLACDecoder>(),
        DecoderImplFor<VorbisDecoder>(),
        DecoderImplFor<MP3Decoder>(),
        DecoderImplFor<ModPlugDecoder>()
    };
    // clang-format on

    /* probe the decoders */

    for (auto& possibleItem : possible)
    {
        stream->Seek(0);
        possibleItem.probeScore = possibleItem.Probe(stream);

        if (possibleItem.probeScore > 0)
            active.push_back(possibleItem);
    }

    std::sort(active.begin(), active.end(), compareProbeScore);

    std::string decodingErrors = "Failed to determine file type:\n";

    for (auto& item : active)
    {
        try
        {
            stream->Seek(0);
            Decoder* decoder = item.Create(stream, bufferSize);

            return decoder;
        }
        catch (love::Exception& e)
        {
            decodingErrors += e.what() + std::string("\n");
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
