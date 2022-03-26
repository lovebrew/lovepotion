#pragma once

#include "common/strongref.h"

#include "objects/video/sync/framesync.h"
#include "objects/video/utility/stream.h"

namespace love
{
    class VideoStream : public Stream
    {
      public:
        static love::Type type;

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

        /* data structure */
        struct Frame
        {
            Frame();

            ~Frame();

            int yw, yh;
            unsigned char* yplane;

            int cw, ch;
            unsigned char* cbplane;
            unsigned char* crplane;
        };

      protected:
        StrongReference<FrameSync> frameSync;
    };
} // namespace love