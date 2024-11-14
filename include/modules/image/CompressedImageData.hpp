#pragma once

#include "common/Data.hpp"
#include "common/Map.hpp"
#include "common/int.hpp"
#include "common/pixelformat.hpp"

#include "modules/image/CompressedSlice.hpp"
#include "modules/image/FormatHandler.hpp"

#include <list>
#include <vector>

namespace love
{
    class CompressedImageData : public Data
    {
      public:
        static Type type;

        CompressedImageData(const std::list<FormatHandler*>& formats, Data* fileData);

        CompressedImageData(const CompressedImageData& other);

        virtual ~CompressedImageData();

        CompressedImageData* clone() const override;

        void* getData() const override;

        size_t getSize() const override;

        int getMipmapCount() const;

        int getSliceCount(int mipmap = 0) const;

        size_t getSize(int mipmap) const;

        void* getData(int mipmap) const;

        int getWidth(int mipmap = 0) const;

        int getHeight(int mipmap = 0) const;

        PixelFormat getFormat() const;

        void setLinear(bool linear);

        bool isLinear() const;

        CompressedSlice* getSlice(int slice, int mipmap) const;

      protected:
        PixelFormat format;
        StrongRef<ByteData> memory;
        CompressedSlices dataImages;

        void checkSliceExists(int slice, int mipmap) const;
    };
} // namespace love
