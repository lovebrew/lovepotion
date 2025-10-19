#include "modules/audio/null/Source.hpp"

namespace love
{
    namespace audio
    {
        namespace null
        {
            Source::Source() : SourceBase(TYPE_STATIC)
            {}

            Source::~Source()
            {}

            SourceBase* Source::clone()
            {
                this->retain();
                return this;
            }

            bool Source::play()
            {
                return false;
            }

            void Source::stop()
            {}

            void Source::pause()
            {}

            bool Source::isPlaying() const
            {
                return false;
            }

            bool Source::isFinished() const
            {
                return true;
            }

            bool Source::update()
            {
                return false;
            }

            void Source::setPitch(float pitch)
            {
                this->pitch = pitch;
            }

            float Source::getPitch() const
            {
                return pitch;
            }

            void Source::setVolume(float volume)
            {
                this->volume = volume;
            }

            float Source::getVolume() const
            {
                return volume;
            }

            void Source::seek(double, Source::Unit)
            {}

            double Source::tell(Source::Unit)
            {
                return 0.0f;
            }

            double Source::getDuration(Unit)
            {
                return -1.0f;
            }

            void Source::setLooping(bool looping)
            {
                this->looping = looping;
            }

            bool Source::isLooping() const
            {
                return looping;
            }

            void Source::setMinVolume(float volume)
            {
                this->minVolume = volume;
            }

            float Source::getMinVolume() const
            {
                return this->minVolume;
            }

            void Source::setMaxVolume(float volume)
            {
                this->maxVolume = volume;
            }

            float Source::getMaxVolume() const
            {
                return this->maxVolume;
            }

            int Source::getChannelCount() const
            {
                return 2;
            }

            int Source::getFreeBufferCount() const
            {
                return 0;
            }

            bool Source::queue(void*, size_t, int, int, int)
            {
                return false;
            }
        } // namespace null
    } // namespace audio
} // namespace love
