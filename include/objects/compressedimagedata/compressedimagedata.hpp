#pragma once

#include <common/data.hpp>
#include <common/pixelformat.hpp>

#include <objects/compressedimagedata/compressedslice.hpp>
#include <utilities/formathandler/formathandler.hpp>

#include <list>
#include <vector>

namespace love
{
    class CompressedImageData : public Data
    {
      public:
        static Type type;

        CompressedImageData(const std::list<FormatHandler*>& formats, Data* fileData);

        CompressedImageData(const CompressedImageData& data);

        virtual ~CompressedImageData();

        CompressedImageData* Clone() const override;

        void* GetData() const override;

        size_t GetSize() const override;

        int GetMipmapCount() const;

        int GetSliceCount(int mipmap = 0) const;

        size_t GetSize(int mipmap) const;

        void* GetData(int mipmap) const;

        int GetWidth(int mipmap = 0) const;

        int GetHeight(int mipmap = 0) const;

        PixelFormat GetFormat() const;

        bool IsSRGB() const;

        CompressedSlice* GetSlice(int slice, int mipmap) const;

      protected:
        PixelFormat format;
        bool sRGB;

        StrongReference<ByteData> memory;
        std::vector<StrongReference<CompressedSlice>> images;

        void CheckSliceExists(int slice, int mipmap) const;
    };
} // namespace love
