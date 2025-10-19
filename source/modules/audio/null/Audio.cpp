#include "modules/audio/null/Audio.hpp"

namespace love
{
    namespace audio
    {
        namespace null
        {
            Audio::Audio() : AudioBase("love.audio.null")
            {}

            Audio::~Audio()
            {}

            SourceBase* Audio::newSource(Decoder* decoder) const
            {
                return new Source();
            }

            SourceBase* Audio::newSource(SoundData* data) const
            {
                return new Source();
            }

            SourceBase* Audio::newSource(int, int, int, int) const
            {
                return new Source();
            }

            int Audio::getActiveSourceCount() const
            {
                return 0;
            }

            int Audio::getMaxSources() const
            {
                return 0;
            }

            bool Audio::play(SourceBase*)
            {
                return false;
            }

            bool Audio::play(const std::vector<SourceBase*>&)
            {
                return false;
            }

            void Audio::stop(SourceBase*)
            {}

            void Audio::stop(const std::vector<SourceBase*>&)
            {}

            void Audio::stop()
            {}

            void Audio::pause(SourceBase*)
            {}

            void Audio::pause(const std::vector<SourceBase*>&)
            {}

            std::vector<SourceBase*> Audio::pause()
            {
                return {};
            }

            void Audio::setVolume(float volume)
            {
                this->volume = volume;
            }

            float Audio::getVolume() const
            {
                return this->volume;
            }

            const std::vector<RecordingDeviceBase*>& Audio::getRecordingDevices()
            {
                return this->capture;
            }
        } // namespace null
    } // namespace audio

} // namespace love
