#pragma once

#include "driver/audiodrv.h"
#include "modules/thread/types/lock.h"

#include <atomic>
#include <map>
#include <queue>

namespace love
{
    namespace common
    {
        class Source;
    }

    class Pool
    {
      public:
        Pool();

        ~Pool();

        int GetActiveSourceCount() const;

        int GetMaxSources() const;

        bool IsRunning();

        bool IsPlaying(common::Source* source);

        bool AssignSource(common::Source* source, size_t& channel, char& wasPlaying);

        void AddSource(common::Source*, size_t channel);

        bool FindSource(common::Source* source, size_t& channel);

        bool ReleaseSource(common::Source* source, bool stop = true);

        void Finish();

        void Update();

        void Sleep();

        thread::Lock Lock();

        std::vector<common::Source*> GetPlayingSources();

      private:
        friend class common::Source;

        std::atomic<bool> running = true;
        std::map<common::Source*, size_t> playing;
        thread::MutexRef mutex;

        std::queue<size_t> available;
        const size_t TOTAL_CHANNELS = 24;
    };
} // namespace love
