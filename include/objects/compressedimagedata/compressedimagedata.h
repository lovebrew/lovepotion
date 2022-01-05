#pragma once

#include "common/data.h"
#include "common/pixelformat.h"

#include "objects/compressedimagedata/types/compressedslice.h"
#include "objects/imagedata/types/formathandler.h"

#include <list>
#include <vector>

namespace love
{
    class CompressedImageData : public Data
    {
      public:
        static love::Type type;

        CompressedImageData(const std::list<FormatHandler*>& formats, Data* fileData);

        CompressedImageData(const CompressedImageData& other);

        virtual ~CompressedImageData();

        CompressedImageData* Clone() const override;

        void* GetData() const override;

        size_t GetSize() const override;

        int GetMipmapCount() const;

        int GetSliceCount(int mip = 0) const;

        size_t GetSize(int mipLevel) const;

        void* GetData(int mipLevel) const;

        int GetWidth(int mipLevel = 0) const;

        int GetHeight(int mipLevel = 0) const;

        PixelFormat GetFormat() const;

        bool IsSRGB() const;

        CompressedSlice* GetSlice(int slice, int mipLevel) const;

      protected:
        PixelFormat format;
        bool sRGB;

        StrongReference<CompressedMemory> memory;
        std::vector<StrongReference<CompressedSlice>> images;

        void CheckSliceExists(int slice, int mipLevel) const;
    };
} // namespace love
