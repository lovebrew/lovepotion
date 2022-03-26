#pragma once

#include <citro2d.h>

#include "objects/video/theora/theorastreamc.h"

namespace love
{
    class TheoraStream : common::TheoraStream
    {
        TheoraStream(File* file);

        struct Frame
        {
            Frame();

            ~Frame();

            C2D_Image image;
            C3D_Tex buffer[2];

            bool currentBuffer;
            th_pixel_fmt format;

            int width, height;
        };

        virtual const void* GetFrontBuffer() const override;

        virtual size_t GetSize() const override;

        virtual bool SwapBuffers() override;

        virtual void ThreadedFillBackBuffer(double dt) override;

        virtual void DeleteBuffers() override;

      protected:
        virtual void ParseHeader() override;

      private:
        Handle handle;
        Frame* frame;
        Tex3DS_SubTexture subTexture;
    };
} // namespace love
