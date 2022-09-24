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
        class DataBuffer : public Object
        {
          public:
            DataBuffer(void* data, size_t size)
            {
                this->buffer = std::make_unique<int16_t[]>(size);
                std::copy_n((int16_t*)data, size, this->buffer.get());
            }

            inline int16_t* GetBuffer() const
            {
                return this->buffer.get();
            }

            inline size_t GetSize() const
            {
                return this->size;
            }

          private:
            std::unique_ptr<int16_t[]> buffer;
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
