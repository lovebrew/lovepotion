#pragma once

#include "common/Module.hpp"
#include "modules/filesystem/File.tcc"
#include "modules/video/VideoStream.hpp"

namespace love
{
    class VideoBase : public Module
    {
      public:
        virtual ~VideoBase()
        {}

        virtual VideoStream* newVideoStream(FileBase* file) const = 0;

      protected:
        VideoBase(const char* name) : Module(M_VIDEO, name)
        {}
    };
} // namespace love
