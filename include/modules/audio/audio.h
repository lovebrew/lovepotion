#pragma once

#include "common/module.h"
#include "objects/source/source.h"

#include "modules/thread/types/threadable.h"
#include "modules/audio/pool/pool.h"

#if defined (_3DS)
    #define AUDIO_THREAD_STACK_SIZE 0x1000
#elif defined (__SWITCH__)
    #define AUDIO_THREAD_STACK_SIZE 0x8000
#endif

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

        private:
            Pool * pool;

            class PoolThread : public Threadable
            {
                public:
                    PoolThread(Pool * pool);

                    virtual ~PoolThread();

                    void SetFinish();

                    void ThreadFunction();

                protected:
                    Pool * pool;
                    std::atomic<bool> finish;
            };

            float volume = 1.0f;

            PoolThread * poolThread;
    };
}
