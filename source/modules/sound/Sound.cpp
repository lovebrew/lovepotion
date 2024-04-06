#include "modules/sound/Sound.hpp"

#include "modules/sound/lullaby/FLACDecoder.hpp"
#include "modules/sound/lullaby/MP3Decoder.hpp"
#include "modules/sound/lullaby/ModPlugDecoder.hpp"
#include "modules/sound/lullaby/VorbisDecoder.hpp"
#include "modules/sound/lullaby/WaveDecoder.hpp"

namespace love
{
    struct DecoderImpl
    {
        Decoder* (*create)(Stream* stream, int bufferSize);
    };

    template<typename DecoderType>
    DecoderImpl DecoderImplFor()
    {
        DecoderImpl decoderImpl;

        decoderImpl.create = [](Stream* stream, int bufferSize) -> Decoder* {
            return new DecoderType(stream, bufferSize);
        };

        return decoderImpl;
    }

    Sound::Sound() : Module(M_SOUND, "love.sound")
    {}

    Sound::~Sound()
    {}

    SoundData* Sound::newSoundData(Decoder* decoder) const
    {
        return new SoundData(decoder);
    }

    SoundData* Sound::newSoundData(int samples, int sampleRate, int bitDepth, int channels) const
    {
        return new SoundData(samples, sampleRate, bitDepth, channels);
    }

    SoundData* Sound::newSoundData(void* data, int samples, int sampleRate, int bitDepth,
                                   int channels) const
    {
        return new SoundData(data, samples, sampleRate, bitDepth, channels);
    }

    Decoder* Sound::newDecoder(Stream* stream, int bufferSize) const
    {
        // clang-format off
        std::array<DecoderImpl, 5> possibleDecoders =
        {
            DecoderImplFor<WaveDecoder>(),
            DecoderImplFor<FLACDecoder>(),
            DecoderImplFor<VorbisDecoder>(),
            DecoderImplFor<MP3Decoder>(),
            DecoderImplFor<ModPlugDecoder>(),
        };
        // clang-format on

        std::string errors = "Failed to determine file type:\n";

        for (const DecoderImpl& decoderImpl : possibleDecoders)
        {
            try
            {
                stream->seek(0);
                auto* decoder = decoderImpl.create(stream, bufferSize);
                return decoder;
            }
            catch (love::Exception& e)
            {
                errors += e.what();
                errors += "\n";
            }
        }

        throw love::Exception("No suitable audio decoders found.\n{:s}", errors);
        return nullptr;
    }
} // namespace love
