#pragma once

#include <common/module.hpp>

#include <utilities/decoder/decoder.hpp>
#include <utilities/stream/stream.hpp>

#include <objects/data/sounddata/sounddata.hpp>

namespace love
{
    class Sound : public Module
    {
      public:
        virtual ~Sound();

        ModuleType GetModuleType() const override
        {
            return M_SOUND;
        }

        const char* GetName() const override
        {
            return "love.sound";
        }

        Decoder* NewDecoder(Stream* stream, int bufferSize);

        SoundData* NewSoundData(Decoder* decoder);

        SoundData* NewSoundData(int samples, int sampleRate, int bitDepth, int channels);

        SoundData* NewSoundData(void* data, int samples, int sampleRate, int bitDepth,
                                int channels);
    };
} // namespace love
