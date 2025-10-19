#include "common/Map.hpp"
#include "modules/audio/RecordingDevice.tcc"

#include <3ds.h>
namespace love
{
    class RecordingDevice final : public RecordingDeviceBase
    {
      public:
        enum SampleRate
        {
            SampleRate_32730 = 32730,
            SampleRate_16360 = 16360,
            SampleRate_10910 = 10910,
            SampleRate_8180  = 8180,
        };

        RecordingDevice(const char* name);

        ~RecordingDevice() override;

        bool start(int samples, int sampleRate, int bitDepth, int channels) override;

        void stop() override;

        SoundData* getData() override;

        const char* getName() const override;

        int getSampleCount() const override;

        int getMaxSamples() const override;

        int getSampleRate() const override;

        int getBitDepth() const override;

        int getChannelCount() const override;

        bool isRecording() const override;

        void setGain(uint8_t gain) override;

        uint8_t getGain() const override;

        static MICU_Encoding getFormat(int bitDepth, int channels);

        // clang-format off
        ENUMMAP_DECLARE(SampleRates, SampleRate, MICU_SampleRate,
          { SampleRate_32730, MICU_SAMPLE_RATE_32730 },
          { SampleRate_16360, MICU_SAMPLE_RATE_16360 },
          { SampleRate_10910, MICU_SAMPLE_RATE_10910 },
          { SampleRate_8180,  MICU_SAMPLE_RATE_8180  }
        );
        // clang-format on

      private:
        static constexpr int BUFFER_SIZE = 0x30000;

        static uint8_t* BUFFER;
        static uint32_t POSITION;

        int samples    = DEFAULT_SAMPLES;
        int sampleRate = DEFAULT_SAMPLE_RATE;
        int bitDepth   = DEFAULT_BIT_DEPTH;
        int channels   = DEFAULT_CHANNELS;

        std::string name;
    };
} // namespace love
