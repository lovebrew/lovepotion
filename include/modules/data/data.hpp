#pragma once

#include <common/module.hpp>

#include <objects/data/bytedata/bytedata.hpp>
#include <objects/data/compresseddata/compresseddata.hpp>
#include <objects/data/dataview/dataview.hpp>

#include <utilities/hashfunction/hashfunction.hpp>

namespace love
{
    class DataModule : public Module
    {
      public:
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

        ModuleType GetModuleType() const override
        {
            return M_DATA;
        }

        const char* GetName() const override
        {
            return "love.data";
        }

        char* Decode(EncodeFormat format, const char* source, size_t sourceLength,
                     size_t& destinationLength);

        char* Decompress(Compressor::Format format, const char* compressedBytes,
                         size_t compressedSize, size_t& rawSize);

        char* Decompress(CompressedData* data, size_t& decompressedSize);

        CompressedData* Compress(Compressor::Format format, const char* rawBytes, size_t rawSize,
                                 int level = -1);

        char* Encode(EncodeFormat format, const char* source, size_t sourceLength,
                     size_t& destinationLength, size_t lineLength = 0);

        void Hash(HashFunction::Function function, Data* input, HashFunction::Value& output);

        void Hash(HashFunction::Function function, const char* input, uint64_t size,
                  HashFunction::Value& output);

        ByteData* NewByteData(size_t size);

        ByteData* NewByteData(const void* data, size_t size);

        ByteData* NewByteData(void* data, size_t size, bool own);

        DataView* NewDataView(Data* data, size_t offset, size_t size);

        static bool GetConstant(ContainerType in, const char*& out);
        static bool GetConstant(const char* in, ContainerType& out);
        static std::vector<const char*> GetConstants(ContainerType);

        static bool GetConstant(EncodeFormat in, const char*& out);
        static bool GetConstant(const char* in, EncodeFormat& out);
        static std::vector<const char*> GetConstants(EncodeFormat);

      private:
        std::string Hash(HashFunction::Function function, Data* input);

        std::string Hash(HashFunction::Function function, const char* input, uint64_t size);
    };
} // namespace love
