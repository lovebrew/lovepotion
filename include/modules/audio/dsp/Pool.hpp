#pragma once

#include "modules/audio/Source.hpp"

#include <map>
#include <mutex>
#include <queue>
#include <vector>

namespace love
{
    namespace audio
    {
        namespace dsp
        {
            class Source;

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

              private:
                friend class Source;

                std::unique_lock<std::recursive_mutex> lock();

                std::vector<SourceBase*> getPlayingSources();

                bool releaseSource(Source* source, bool stop = true);

                void addSource(Source* source, size_t channel);

                bool assignSource(Source* source, size_t& channel, uint8_t& wasPlaying);

                bool findSource(Source* source, size_t& channel);

                int totalSources;
                std::queue<size_t> available;
                std::map<Source*, size_t> playing;
                std::recursive_mutex mutex;
            };
        } // namespace dsp
    } // namespace audio

} // namespace love
