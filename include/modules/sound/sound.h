#pragma once

#include "objects/decoder/decoder.h"
#include "objects/filedata/filedata.h"
#include "objects/sounddata/sounddata.h"

#include "flacdecoder.h"
#include "modplugdecoder.h"
#include "mp3decoder.h"
#include "vorbisdecoder.h"
#include "wavedecoder.h"

#include "common/module.h"

namespace love
{
    class Sound : public Module
    {
      public:
        static love::Type type;

        ~Sound();

        ModuleType GetModuleType() const
        {
            return M_SOUND;
        }

        const char* GetName() const override
        {
            return "love.sound";
        }

        Decoder* NewDecoder(love::FileData* data, int bufferSize);

        SoundData* NewSoundData(Decoder* decoder);

        SoundData* NewSoundData(int samples, int sampleRate, int bitDepth, int channels);

        SoundData* NewSoundData(void* data, int samples, int sampleRate, int bitDepth,
                                int channels);
    };
} // namespace love
