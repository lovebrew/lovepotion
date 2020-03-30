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

            // std::vector<Source *> Pause();

            void Pause();

            void SetVolume(float volume);

            float GetVolume() const;

            void AddSourceToPool(Source * source);

            static std::array<bool, 24> channels;

            static int GetOpenChannel();

            static void FreeChannel(size_t channel);

            static void ReleaseSource(Source * source, bool stop = true);

        private:
            float volume = 1.0f;

            bool driverInit = false;
            bool audioInit = false;

            Thread poolThread;

            std::vector<Source *> pool;
    };
}

extern AudioDriver g_AudioDriver;
extern Mutex g_audrvMutex;
