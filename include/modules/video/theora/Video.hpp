#pragma once

#include "common/StrongRef.hpp"
#include "modules/filesystem/File.tcc"
#include "modules/thread/Threadable.hpp"
#include "modules/video/Video.hpp"
#include "modules/video/VideoStream.hpp"
#include "modules/video/theora/TheoraVideoStream.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace love
{
    class Worker;

    class VideoModule : public VideoBase
    {
      public:
        VideoModule();
        virtual ~VideoModule();

        VideoStream* newVideoStream(FileBase* file) const;

      private:
        Worker* worker;
    };

    class Worker : public Threadable
    {
      public:
        Worker();
        virtual ~Worker();

        void run();

        void addStream(TheoraVideoStream* stream);

        void stop();

      private:
        std::vector<StrongRef<TheoraVideoStream>> streams;
        std::mutex mutex;
        std::condition_variable_any condition;
        std::atomic<bool> stopping;
    };
} // namespace love
