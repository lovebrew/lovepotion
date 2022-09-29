#pragma once

#include <objects/source/source.tcc>
#include <utilities/driver/dsp_ext.hpp>

#include <objects/data/sounddata/sounddata.hpp>
#include <utilities/decoder/decoder.hpp>

#include <queue>

namespace love
{
    class Audio;
    class AudioPool;

    template<>
    class Source<Console::CAFE> : public Source<Console::ALL>
    {
      public:
        Source(AudioPool* pool, SoundData* soundData);

        Source(AudioPool* pool, Decoder* decoder);

        Source(AudioPool* pool, int sampleRate, int bitDepth, int channels, int buffers);

        Source(const Source& other);

        virtual ~Source();

        virtual Source* Clone();

        /* normal stuff */

        bool Play();

        void Stop();

        void Pause();

        bool IsPlaying() const;

        bool IsFinished() const;

        bool Update();

        void SetVolume(float volume);

        float GetVolume() const;

        void Seek(double offset, Unit unit);

        double Tell(Unit unit);

        double GetDuration(Unit unit);

        void SetLooping(bool looping);

        int GetChannelCount() const;

        int GetFreeBufferCount() const;

        bool Queue(void* data, size_t length, int sampleRate, int bitDepth, int channels);

        /* atomic things */

        void PrepareAtomic();

        void TeardownAtomic();

        bool PlayAtomic(AXWaveBuf& waveBuffer);

        void StopAtomic();

        void PauseAtomic();

        void ResumeAtomic();

        /* global state stuff */

        static bool Play(const std::vector<Source<Console::Which>*>& sources);

        static void Stop(const std::vector<Source<Console::Which>*>& sources);

        static void Pause(const std::vector<Source<Console::Which>*>& sources);

        static std::vector<Source<Console::Which>*> Pause(AudioPool* pool);

        static void Stop(AudioPool* pool);

      private:
        static constexpr size_t MAX_BUFFERS = 2;

        void Reset();

        int StreamAtomic(AXWaveBuf& buffer, Decoder* decoder);

        AudioPool* pool;

        AXWaveBuf buffers[2];

        StrongReference<Decoder> decoder;
    };
} // namespace love
