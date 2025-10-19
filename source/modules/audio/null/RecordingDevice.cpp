#include "modules/audio/null/RecordingDevice.hpp"
#include "modules/audio/null/Audio.hpp"

namespace love
{
    namespace audio
    {
        namespace null
        {
            const char* RecordingDevice::name = "null";

            RecordingDevice::RecordingDevice(const char*)
            {}

            RecordingDevice::~RecordingDevice()
            {}

            bool RecordingDevice::start(int, int, int, int)
            {
                return false;
            }

            void RecordingDevice::stop()
            {}

            SoundData* RecordingDevice::getData()
            {
                return nullptr;
            }

            int RecordingDevice::getSampleCount() const
            {
                return 0;
            }

            int RecordingDevice::getMaxSamples() const
            {
                return 0;
            }

            int RecordingDevice::getSampleRate() const
            {
                return 0;
            }

            int RecordingDevice::getBitDepth() const
            {
                return 0;
            }

            int RecordingDevice::getChannelCount() const
            {
                return 0;
            }

            const char* RecordingDevice::getName() const
            {
                return name;
            }

            bool RecordingDevice::isRecording() const
            {
                return false;
            }
        } // namespace null
    } // namespace audio
} // namespace love
