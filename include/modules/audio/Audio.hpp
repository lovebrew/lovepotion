#pragma once

#include "common/Module.hpp"

#include "modules/audio/Source.hpp"
#include "modules/thread/Threadable.hpp"

namespace love
{
    class Audio : public Module
    {
      public:
        Audio();

        ~Audio();

        Source* newSource(SoundData* soundData) const;

        Source* newSource(Decoder* decoder) const;

        Source* newSource(int sampleRate, int bitDepth, int channels, int buffers) const;

        int getActiveSourceCount() const;

        int getMaxSources() const;

        bool play(Source* source);

        void stop();

        void stop(Source* source);

        void pause(Source* source);

        void setVolume(float volume);

        float getVolume() const;

        static bool play(const std::vector<Source*>& sources);

        static void stop(const std::vector<Source*>& sources);

        static void pause(const std::vector<Source*>& sources);

        std::vector<Source*> pause();

      private:
        Pool* pool;

        class PoolThread : public Threadable
        {
          public:
            PoolThread(Pool* pool);

            void setFinish()
            {
                this->finish = true;
            }

            void run();

          protected:
            Pool* pool;
            std::atomic<bool> finish;
            std::recursive_mutex mutex;
        };

        PoolThread* poolThread;
    };
} // namespace love
