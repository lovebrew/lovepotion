#pragma once

#include "common/threads/lock.h"

#if defined (_3DS)
    typedef ndspCallback POOL_CALLBACK;
#elif defined (__SWITCH__)
    typedef ThreadFunc POOL_CALLBACK;
#endif

namespace love
{
    class Source;

    class Pool
    {
        public:
            Pool();

            bool IsRunning();

            bool IsPlaying(Source * source);

            bool AssignSource(Source * source, size_t & channel, bool & wasPlaying);
            bool FindSource(Source * source, size_t & channel);
            bool ReleaseSource(Source * source, bool stop = true);

            void Finish();
            void Update();

            void Sleep();

            thread::Lock Lock();

        private:
            friend class Source;

            std::atomic<bool> running = true;
            std::map<Source *, size_t> playing;
            thread::MutexRef mutex;

            std::queue<size_t> available;
            const size_t TOTAL_CHANNELS = 24;
    };
}
