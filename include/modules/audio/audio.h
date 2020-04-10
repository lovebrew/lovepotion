#pragma once

#include "objects/source/source.h"

#define AUDIO_THREAD_STACK_SIZE 0x8000

namespace love
{
    class Audio : public Module
    {
        public:
            ModuleType GetModuleType() const { return M_AUDIO; }
            const char * GetName() const override { return "love.audio"; }

            Audio();
            ~Audio();

            void _Initialize();
            void _Exit();

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

            static std::atomic<bool> THREAD_RUN;

        private:
            float volume = 1.0f;

            Thread poolThread;
            Pool * pool;

            static void AudioThreadRunner(void * arg)
            {
                Pool * pool = (Pool *)arg;

                while (THREAD_RUN)
                {
                    pool->Update();

                    pool->Sleep();
                }
            }
    };
}
