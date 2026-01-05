#pragma once

#include "modules/filesystem/File.tcc"
#include "modules/thread/Threadable.hpp"
#include "modules/video/Video.hpp"

#include "modules/video/VideoStream.hpp"
#include "modules/video/theora/TheoraVideoStream.hpp"

#include <thread>
#include <vector>

namespace love
{
    class Worker;

    class VideoModule final : public VideoModuleBase
    {
      public:
        VideoModule();

        virtual ~VideoModule();

        VideoStream* newVideoStream(FileBase* file);

      private:
        Worker* worker;
    };

    class Worker : public Threadable
    {
      public:
        Worker();

        virtual ~Worker();

        void run() override;

        void addStream(TheoraVideoStream* stream);

        void stop();

      private:
        std::vector<StrongRef<TheoraVideoStream>> streams;
        std::recursive_mutex mutex;
        std::condition_variable_any condition;
        bool stopping;
    };
} // namespace love
