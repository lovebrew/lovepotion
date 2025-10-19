#include "modules/audio/Audio.hpp"

namespace love
{
    AudioBase::AudioBase(const char* name) : Module(M_AUDIO, name)
    {}

    bool AudioBase::setMixWithSystem(bool)
    {
        return false;
    }
} // namespace love
