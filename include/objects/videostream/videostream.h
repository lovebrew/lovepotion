#pragma once

#include "common/strongref.h"
#include "objects/videostream/sync/framesync.h"

namespace love
{
    class VideoStream : public Object
    {
      public:
        static love::Type type;

        struct IFrame
        {
            IFrame()
            {}

            ~IFrame()
            {}
        };

        virtual ~VideoStream()
        {}

        virtual int GetWidth() const = 0;

        virtual int GetHeight() const = 0;

        virtual const std::string& GetFilename() const = 0;

        /* playback api */

        virtual void Play();

        virtual void Pause();

        virtual void Seek(double offset);

        virtual double Tell() const;

        virtual bool IsPlaying() const;

        /* sync stuff */

        virtual void SetSync(FrameSync* sync);

        virtual FrameSync* GetSync() const;

        virtual void FillBackBuffer()
        {}

        virtual const void* GetFrontBuffer() const = 0;

        virtual size_t GetSize() const = 0;

        virtual bool SwapBuffers() = 0;

      protected:
        StrongReference<FrameSync> frameSync;
    };
} // namespace love
