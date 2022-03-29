#pragma once

#include "objects/videostream/theora/theorastreamc.h"

namespace love
{
    class TheoraStream : public common::TheoraStream
    {
      public:
        TheoraStream(File* file);

        ~TheoraStream();

        virtual const void* GetFrontBuffer() const override;

        virtual size_t GetSize() const override;

        virtual bool SwapBuffers() override;

        virtual void ThreadedFillBackBuffer(double dt) override;

        struct Frame
        {
            Frame();

            ~Frame();

            int yw, yh;
            uint8_t* yPlane;

            int cw, ch;
            uint8_t* cbPlane;
            uint8_t* crPlane;
        };

      protected:
        virtual void ParseHeader() override;

      private:
        Frame* frontBuffer;
        Frame* backBuffer;

        unsigned yPlaneXOffset;
        unsigned cPlaneXOffset;
        unsigned yPlaneYOffset;
        unsigned cPlaneYOffset;
    };
} // namespace love
