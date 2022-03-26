#pragma once

#include "common/module.h"
#include "objects/file/file.h"
#include "objects/video/videostream.h"

namespace love
{
    class Worker;

    class Video : public Module
    {
        virtual ~Video()
        {}

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
