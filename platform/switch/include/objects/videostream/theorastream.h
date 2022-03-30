#pragma once

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

            int yw, yh;
            uint8_t* yPlane;

            int cw, ch;
            uint8_t* cbPlane;
            uint8_t* crPlane;
        };

        virtual size_t GetSize() const override;

        virtual void SetupBuffers() override;

        virtual void FillBufferData(th_ycbcr_buffer bufferInfo) override;

      private:
        unsigned yPlaneXOffset;
        unsigned cPlaneXOffset;
        unsigned yPlaneYOffset;
        unsigned cPlaneYOffset;
    };
} // namespace love
