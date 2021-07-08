#pragma once

#include "common/module.h"
#include "objects/source/source.h"

#include "modules/audio/pool/pool.h"
#include "modules/thread/types/threadable.h"

#include "driver/audiodrv.h"
#include <memory>

namespace love
{
    class Audio : public Module
    {
      public:
        ModuleType GetModuleType() const
        {
            return M_AUDIO;
        }

        const char* GetName() const override
        {
            return "love.audio";
        }

        Audio();

        virtual ~Audio();

        int GetActiveSourceCount() const;

        int GetMaxSources() const;

        Source* NewSource(SoundData* data);

        Source* NewSource(Decoder* decoder);

        bool Play(Source* source);

        bool Play(const std::vector<common::Source*>& sources);

        bool Play();

        void Stop(Source* source);

        void Stop(const std::vector<common::Source*>& sources);

        void Stop();

        void Pause(const std::vector<common::Source*>& sources);

        void Pause(Source* source);

        std::vector<common::Source*> Pause();

        void SetVolume(float volume);

        float GetVolume() const;

      private:
        Pool* pool;

        class PoolThread : public Threadable
        {
          public:
            PoolThread(Pool* pool);

            virtual ~PoolThread();

            void SetFinish();

            void ThreadFunction();

          protected:
            Pool* pool;
            std::atomic<bool> finish;
        };

        float volume = 1.0f;

        PoolThread* poolThread;
    };
} // namespace love
