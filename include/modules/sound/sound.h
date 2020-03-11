#pragma once

#include "objects/decoder/decoder.h"
#include "objects/sounddata/sounddata.h"
#include "objects/filedata/filedata.h"

#include "vorbisdecoder.h"

namespace love
{
    class Sound : public Module
    {
        public:
            static love::Type type;

            ~Sound();

            ModuleType GetModuleType() const { return M_SOUND; }

            const char * GetName() const override { return "love.sound"; }

            Decoder * NewDecoder(love::FileData * data, int bufferSize);

            SoundData * NewSoundData(Decoder * decoder);
    };
}
