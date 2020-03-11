#pragma once

#include "objects/source/source.h"

namespace love
{
    class Audio : public Module
    {
        public:
            ModuleType GetModuleType() const { return M_AUDIO; }
            const char * GetName() const override { return "love.audio"; }

            Audio();
            ~Audio();

            Source * NewSource(SoundData * data);
            Source * NewSource(Decoder * decoder);

            bool Play(Source * source);

            bool Play(const std::vector<Source *> & sources);

            void Stop(Source * source);

            void Stop(const std::vector<Source *> & sources);

            void Stop();

            void Pause(Source * source);

            void Pause (const std::vector<Source *> & sources);

            void SetVolume(float volume);

            float GetVolume() const;


        private:
            std::vector<Source *> pool;
    };
}
