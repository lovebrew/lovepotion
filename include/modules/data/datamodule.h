#pragma once

#include "common/stringmap.h"

#include "objects/data/byte/bytedata.h"
#include "objects/data/compressed/compresseddata.h"
#include "objects/data/view/dataview.h"

#include "common/base64.h"

namespace love
{
    namespace data
    {
        enum EncodeFormat
        {
            ENCODE_BASE64,
            ENCODE_HEX,
            ENCODE_MAX_ENUM
        };

        enum ContainerType
        {
            CONTAINER_DATA,
            CONTAINER_STRING,
            CONTAINER_MAX_ENUM
        };

        char * _Encode(EncodeFormat format, const char * src, size_t srcLength, size_t & dstLength, size_t lineLength = 0);

        char * _Decode(EncodeFormat format, const char * src, size_t srcLength, size_t & dstLength);

        CompressedData * _Compress(Compressor::Format format, const char * rawBytes, size_t rawSize, int level = -1);

        char * _Decompress(Compressor::Format format, const char * compresssedBytes, size_t compressedSize, size_t & rawSize);

        char * _Decompress(CompressedData * data, size_t & decompressedSize);
    }

    class DataModule : public Module
    {
        public:
            ByteData * NewByteData(size_t size);
            ByteData * NewByteData(const void * data, size_t size);
            ByteData * NewByteData(void * data, size_t size, bool own);

            DataView * NewDataView(Data * data, size_t offset, size_t size);

            ModuleType GetModuleType() const { return M_DATA; }

            const char * GetName() const override { return "love.data"; }

            static bool GetConstant(data::ContainerType in, const char *& out);
            static bool GetConstant(const char * in, data::ContainerType & out);
            static std::vector<std::string> GetConstants(data::ContainerType);

            static bool GetConstant(data::EncodeFormat in, const char *& out);
            static bool GetConstant(const char * in, data::EncodeFormat & out);
            static std::vector<std::string> GetConstants(data::EncodeFormat);
    };
}
