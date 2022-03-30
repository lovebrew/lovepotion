#pragma once

#include <citro2d.h>

#include "objects/videostream/theora/theorastreamc.h"

namespace love
{
    class TheoraStream : public common::TheoraStream
    {
      public:
        TheoraStream(File* file);

        struct Frame : VideoStream::IFrame
        {
            Frame();

            ~Frame();

            C3D_Tex* buffer;
            int width, height;
        };

        virtual size_t GetSize() const override;

        virtual void SetupBuffers() override;

        virtual void FillBufferData(th_ycbcr_buffer bufferInfo) override;

      private:
        void SetPostProcessingLevel();

        th_pixel_fmt format;
        int width, height;
        Handle handle;
    };
} // namespace love
