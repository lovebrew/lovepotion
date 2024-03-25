#pragma once

#include "common/Data.hpp"
#include "modules/data/misc/Compressor.hpp"

namespace love
{
    class CompressedData : public Data
    {
      public:
        static Type type;

        CompressedData(Compressor::Format format, char* data, size_t size, size_t rawSize,
                       bool own = true);

        CompressedData(const CompressedData& other);

        virtual ~CompressedData();

        Compressor::Format getFormat() const;

        size_t getDecompressedSize() const;

        CompressedData* clone() const override;

        void* getData() const override;

        size_t getSize() const override;

      private:
        Compressor::Format format;

        char* data;
        size_t dataSize;

        size_t originalSize;
    };
} // namespace love
