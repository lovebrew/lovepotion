#pragma once

#include "common/StrongRef.hpp"
#include "common/int.hpp"
#include "common/pixelformat.hpp"

#include "modules/data/ByteData.hpp"
#include "modules/image/ImageDataBase.hpp"

namespace love
{
    class CompressedSlice : public ImageDataBase
    {
      public:
        CompressedSlice(PixelFormat format, int width, int height, ByteData* memory, size_t offset,
                        size_t size);

        CompressedSlice(const CompressedSlice&);

        virtual ~CompressedSlice();

        CompressedSlice* clone() const override;

        void* getData() const override
        {
            return (uint8_t*)this->memory->getData() + this->offset;
        }

        size_t getSize() const override
        {
            return this->size;
        }

        size_t getOffset() const
        {
            return this->offset;
        }

      private:
        StrongRef<ByteData> memory;
        size_t offset;
        size_t size;
    };
} // namespace love
