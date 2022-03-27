#pragma once

#include "common/module.h"
#include "objects/file/file.h"
#include "objects/videostream/videostream.h"

namespace love
{
    class Worker;

    class VideoModule : public Module
    {
      public:
        VideoModule();

        virtual ~VideoModule();

        virtual const char* GetName() const
        {
            return "love.video";
        }

        virtual ModuleType GetModuleType() const
        {
            return M_VIDEO;
        }

        VideoStream* NewVideoStream(File* file);

      private:
        Worker* workerThread;
    };
} // namespace love
