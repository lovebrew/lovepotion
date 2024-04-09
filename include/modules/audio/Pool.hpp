#pragma once

#include "modules/audio/Source.hpp"

#include <map>
#include <queue>
#include <vector>

namespace love
{
    class Pool
    {
      public:
        Pool();

        ~Pool();

        bool isAvailable();

        bool isPlaying(Source* source);

        void update();

        int getActiveSourceCount() const;

        int getMaxSources() const;

        std::recursive_mutex& getMutex()
        {
            return this->mutex;
        }

        std::unique_lock<std::recursive_mutex> lock();

      private:
        friend class Source;

        static constexpr size_t MAX_SOURCES = 24;
        int totalSources;

        std::queue<size_t> available;
        std::map<Source*, size_t> playing;
        std::recursive_mutex mutex;

        std::vector<Source*> getPlayingSources();

        bool releaseSource(Source* source, bool stop = true);

        void addSource(Source* source, size_t channel);

        bool assignSource(Source* source, size_t& channel, uint8_t& wasPlaying);

        bool findSource(Source* source, size_t& channel);
    };
} // namespace love
