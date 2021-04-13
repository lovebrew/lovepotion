#pragma once

#include "common/data.h"
#include "modules/data/compressor/compressor.h"

namespace love
{
    class CompressedData : public Data
    {
      public:
        static love::Type type;

        CompressedData(Compressor::Format format, char* data, size_t compressedSize, size_t rawSize,
                       bool own = true);
        CompressedData(const CompressedData& other);

        virtual ~CompressedData();

        Compressor::Format GetFormat() const;

        size_t GetDecompressedSize() const;

        CompressedData* Clone() const override;

        void* GetData() const override;

        size_t GetSize() const override;

      private:
        Compressor::Format format;

        char* data;
        size_t dataSize;

        size_t originalSize;
    };
} // namespace love
