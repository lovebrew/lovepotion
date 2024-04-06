#pragma once

#include "common/Module.hpp"
#include "common/Stream.hpp"

#include "modules/sound/Decoder.hpp"
#include "modules/sound/SoundData.hpp"

namespace love
{
    class Sound : public Module
    {
      public:
        Sound();

        virtual ~Sound();

        SoundData* newSoundData(Decoder* decoder) const;

        SoundData* newSoundData(int samples, int sampleRate, int bitDepth, int channels) const;

        SoundData* newSoundData(void* data, int samples, int sampleRate, int bitDepth,
                                int channels) const;

        Decoder* newDecoder(Stream* stream, int bufferSize) const;
    };
} // namespace love
