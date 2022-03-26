#pragma once

#include "common/strongref.h"

#include "objects/source/source.h"
#include "objects/video/sync/framesync.h"

namespace love
{
    class SourceSync : FrameSync
    {
      public:
        SourceSync(Source* source);

        virtual double GetPosition() const override;

        virtual void Play() override;

        virtual void Pause() override;

        virtual void Seek(double time) override;

        virtual bool IsPlaying() const override;

      private:
        StrongReference<Source> source;
    };
} // namespace love
