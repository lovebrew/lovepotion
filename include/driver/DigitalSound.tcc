#pragma once

#include "common/Map.hpp"
#include "common/Singleton.tcc"

#include <atomic>

namespace love
{
    template<class T>
    class DigitalSoundBase : public Singleton<T>
    {
      public:
        enum InterpretedFormat
        {
            FORMAT_MONO   = 0x01,
            FORMAT_STEREO = 0x02,
            FORMAT_MAX_ENUM
        };

        enum EncodingFormat
        {
            ENCODING_PCM8  = 0x08,
            ENCODING_PCM16 = 0x10,
            ENCODING_MAX_ENUM
        };

        DigitalSoundBase() : initialized(false)
        {}

        virtual void initialize() = 0;

        virtual void deInitialize() = 0;

        void update()
        {
            static_cast<T*>(this)->updateImpl();
        }

      protected:
        std::atomic<bool> initialized;
    };
} // namespace love
