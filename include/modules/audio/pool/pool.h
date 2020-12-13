#pragma once

#include "modules/thread/types/lock.h"

#if defined (_3DS)
    #define SLEEP_TIME 5000000
#elif defined (__SWITCH__)
    #define SLEEP_TIME 0
#endif

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

            bool IsRunning();

            bool IsPlaying(common::Source * source);

            bool AssignSource(common::Source * source, size_t & channel, bool & wasPlaying);

            bool FindSource(common::Source * source, size_t & channel);

            bool ReleaseSource(common::Source * source, bool stop = true);

            void Finish();

            void Update();

            void Sleep();

            thread::Lock Lock();

        private:
            friend class common::Source;

            std::atomic<bool> running = true;
            std::map<common::Source *, size_t> playing;
            thread::MutexRef mutex;

            std::queue<size_t> available;
            const size_t TOTAL_CHANNELS = 24;
    };
}
