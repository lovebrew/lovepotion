#pragma once

#include "common/Singleton.tcc"
#include "common/map.hpp"

namespace love
{
    template<class T>
    class DigitalSoundBase : public Singleton<T>
    {
      public:
        enum InterpretedFormat
        {
            FORMAT_MONO,
            FORMAT_STEREO,
            FORMAT_MAX_ENUM
        };

        enum EncodingFormat
        {
            ENCODING_PCM8  = 0x08,
            ENCODING_PCM16 = 0x10,
            ENCODING_MAX_ENUM
        };

        virtual void initialize()
        {}

        void update()
        {
            static_cast<T*>(this)->updateImpl();
        }
    };
} // namespace love
