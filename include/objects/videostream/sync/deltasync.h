#pragma once

#include "modules/thread/types/mutex.h"
#include "objects/videostream/sync/framesync.h"

namespace love
{
    class DeltaSync : public FrameSync
    {
      public:
        DeltaSync();

        ~DeltaSync();

        virtual double GetPosition() const override;

        virtual void Update(double dt) override;

        virtual void Play() override;

        virtual void Pause() override;

        virtual void Seek(double time) override;

        virtual bool IsPlaying() const override;

      private:
        bool playing;
        double position;
        double speed;

        thread::MutexRef mutex;
    };
} // namespace love
