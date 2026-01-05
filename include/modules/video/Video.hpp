#pragma once

#include "common/Module.hpp"
#include "modules/filesystem/File.tcc"

#include "modules/video/VideoStream.hpp"

namespace love
{
    class VideoModuleBase : public Module
    {
      public:
        virtual ~VideoModuleBase()
        {}

        virtual VideoStream* newVideoStream(FileBase* file) = 0;

      protected:
        VideoModuleBase(const char* name) : Module(M_VIDEO, name)
        {}
    };
} // namespace love
