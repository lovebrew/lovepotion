#pragma once

#include <citro2d.h>

#include "objects/videostream/theora/theorastreamc.h"

namespace love
{
    class TheoraStream : public common::TheoraStream
    {
      public:
        TheoraStream(File* file);

        ~TheoraStream()
        {
            delete this->frame;
        }

        struct Frame
        {
            Frame();

            ~Frame();

            C2D_Image image;
            C3D_Tex buffer[2];

            bool currentBuffer;
            th_pixel_fmt format;

            int width, height;

            int postProcess;
            int maxPostProcess;
            int postProcessOffset;
        };

        virtual const void* GetFrontBuffer() const override;

        virtual size_t GetSize() const override;

        virtual bool SwapBuffers() override;

        virtual void ThreadedFillBackBuffer(double dt) override;

      protected:
        virtual void ParseHeader() override;

      private:
        Handle handle;

        Frame* frame;
        Tex3DS_SubTexture subTexture;
    };
} // namespace love
