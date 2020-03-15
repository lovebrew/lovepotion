#pragma once

namespace love
{
    class Source;

    class Pool
    {
        public:
            Pool();
            ~Pool();

            bool IsAvailable() const;

            bool IsPlaying(Source * source);

            void Update();

            int GetActiveSourceCount() const;

            int GetMaxSources() const;

        private:
            friend class Source;

            std::vector<Source *> GetPlayingSources();

            bool ReleaseSource(Source * source, bool stop = true);

            bool AssignSource(Source * source, int & outChannel, bool wasPlaying);

            bool FindSource(Source * source, int & outChannel);

            static const int MAX_SOURCES = 64;

            bool sources[MAX_SOURCES];

            std::queue<bool> available;

            std::map<Source *, bool> playing;
    };
}
