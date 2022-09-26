#pragma once

#include <common/console.hpp>
#include <common/object.hpp>

#include <algorithm>
#include <memory>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class DSP
    {
      public:
        struct DataBuffer
        {
            int16_t* buffer;
            size_t size;
        };

        enum AudioFormat
        {
            FORMAT_MONO,
            FORMAT_STEREO,
            FORMAT_MAX_ENUM
        };

        bool IsInitialized() const
        {
            return this->initialized;
        }

      protected:
        bool initialized;
    };
} // namespace love
