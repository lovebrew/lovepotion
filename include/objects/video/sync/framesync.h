#pragma once

#include "objects/object.h"

namespace love
{
    class FrameSync : public Object
    {
      public:
        virtual ~FrameSync()
        {}

        virtual double GetPosition() const = 0;

        virtual void Update(double)
        {}

        void CopyState(const FrameSync* other);

        /* playback api */

        virtual void Play() = 0;

        virtual void Pause() = 0;

        virtual void Seek(double offset) = 0;

        virtual double Tell() const;

        virtual bool IsPlaying() const = 0;
    };
} // namespace love
