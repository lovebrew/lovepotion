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
            for (auto frame : this->buffers)
                delete frame;
        }

        struct Frame
        {
            Frame();

            ~Frame();

            C3D_Tex buffer;
            C2D_Image texture;
        };

        virtual const void* GetFrontBuffer() const override;

        virtual size_t GetSize() const override;

        virtual bool SwapBuffers() override;

        virtual void ThreadedFillBackBuffer(double dt) override;

      protected:
        virtual void ParseHeader() override;

      private:
        static constexpr int FRONT_BUFFER = 0x00;
        static constexpr int BACK_BUFFER  = 0x01;

        th_pixel_fmt format;

        int width, height;

        int postProcess;
        int maxPostProcess;
        bool currentBuffer;
        int postProcessOffset;

        Handle handle;

        Frame* buffers[2];

        Tex3DS_SubTexture subTexture;
    };
} // namespace love
