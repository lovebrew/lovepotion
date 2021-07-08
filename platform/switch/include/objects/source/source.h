#pragma once

#include "objects/source/sourcec.h"

namespace love
{
    class Source : public common::Source
    {
      public:
        Source(Pool* pool, SoundData* sound);

        Source(Pool* pool, Decoder* decoder);

        Source(const Source& other);

        virtual ~Source();

        Source* Clone();

        void SetLooping(bool should) override;

        bool Update() override;

        bool IsPlaying() const override;

        bool IsFinished() const override;

        void SetVolume(float volume) override;

        void StopAtomic() override;

      protected:
        double GetSampleOffset() override;

        void ClearChannel() override;

      private:
        AudioDriverWaveBuf sources[Source::MAX_BUFFERS];

        void Reset() override;

        void InitializeStreamBuffers(Decoder* decoder) override;

        void PrepareAtomic() override;

        int StreamAtomic(size_t which) override;

        bool PlayAtomic() override;

        void PauseAtomic() override;

        void ResumeAtomic() override;

        void FreeBuffer() override;
    };
} // namespace love
