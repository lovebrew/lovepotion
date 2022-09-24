#pragma once

#include <objects/source_ext.hpp>
#include <utilities/threads/threads.hpp>

#include <map>
#include <queue>
#include <vector>

namespace love
{
    class AudioPool
    {
      public:
        AudioPool();

        ~AudioPool();

        bool IsAvailable();

        bool IsPlaying(Source<Console::Which>* source);

        void Update();

        int GetActiveSourceCount() const;

        int GetMaxSources() const;

      private:
        friend class Source<Console::Which>;
        static constexpr size_t MAX_SOURCES = (Console::Is(Console::CAFE)) ? 6 : 24;

        int totalSources;
        std::queue<size_t> available;
        std::map<Source<Console::Which>*, size_t> playing;
        love::mutex mutex;

        std::unique_lock<love::mutex> Lock();

        std::vector<Source<Console::Which>*> GetPlayingSources();

        bool ReleaseSource(Source<Console::Which>* source, bool stop = true);

        void AddSource(Source<Console::Which>* source, size_t channel);

        bool AssignSource(Source<Console::Which>* source, size_t& channel, uint8_t& wasPlaying);

        bool FindSource(Source<Console::Which>* source, size_t& channel);
    };
} // namespace love
