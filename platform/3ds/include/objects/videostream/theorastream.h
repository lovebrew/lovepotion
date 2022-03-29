#pragma once

#include <citro2d.h>

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

            C3D_Tex* buffer;
            int width, height;
        };

      protected:
        virtual void ParseHeader() override;

      private:
        Frame* frontBuffer;
        Frame* backBuffer;

        th_pixel_fmt format;

        int width, height;

        int postProcess;
        int maxPostProcess;
        bool currentBuffer;
        int postProcessOffset;

        Handle handle;
    };
} // namespace love
