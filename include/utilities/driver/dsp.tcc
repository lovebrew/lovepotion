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

        void Sleep()
        {}

      protected:
        bool initialized;
    };
} // namespace love
