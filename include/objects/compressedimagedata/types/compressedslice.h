#pragma once

#include "objects/compressedimagedata/types/compressedmemory.h"
#include "objects/imagedata/imagedatabase.h"

namespace love
{
    class CompressedSlice : public ImageDataBase
    {
      public:
        CompressedSlice(PixelFormat format, int width, int height, CompressedMemory* memory,
                        size_t offset, size_t size);

        CompressedSlice(const CompressedSlice& slice);

        virtual ~CompressedSlice();

        CompressedSlice* Clone() const override;

        void* GetData() const override
        {
            return this->memory->data + this->offset;
        }

        size_t GetSize() const override
        {
            return this->dataSize;
        }

        bool IsSRGB() const override
        {
            return this->sRGB;
        }

        size_t GetOffset() const
        {
            return this->offset;
        }

      private:
        StrongReference<CompressedMemory> memory;

        size_t offset;
        size_t dataSize;

        bool sRGB;
    };
} // namespace love
