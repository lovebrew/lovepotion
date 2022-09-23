#pragma once

#include <common/console.hpp>
#include <common/object.hpp>

#include <memory>

namespace love
{
    class DataBuffer : public Object
    {
      public:
        DataBuffer(void* data, size_t size);

        virtual ~DataBuffer();

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

      protected:
        bool initialized;
    };
} // namespace love
