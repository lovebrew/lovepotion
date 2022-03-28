#pragma once

#include "modules/thread/types/conditional.h"
#include "modules/thread/types/threadable.h"

#include "objects/videostream/theorastream.h"
#include "objects/videostream/utility/stream.h"

#include <vector>

namespace love
{
    class Worker : public Threadable
    {
      public:
        Worker();

        virtual ~Worker();

        void ThreadFunction();

        void AddStream(TheoraStream* stream);

        void Stop();

      private:
        std::vector<StrongReference<TheoraStream>> streams;

        thread::MutexRef mutex;
        thread::ConditionalRef condition;

        bool stopping;
    };
} // namespace love
