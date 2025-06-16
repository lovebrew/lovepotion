#pragma once

#include "common/Module.hpp"

#include "modules/data/ByteData.hpp"
#include "modules/data/CompressedData.hpp"
#include "modules/data/DataView.hpp"
#include "modules/data/misc/HashFunction.hpp"

#include "common/Map.hpp"

#include <memory>
#include <string>

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

        CompressedData* compress(Compressor::Format format, const char* bytes, size_t size,
                                 int level = -1);

        char* decompress(CompressedData* data, size_t& size);

        char* decompress(Compressor::Format format, const char* bytes, size_t size,
                         size_t& rawSize);

        char* encode(EncodeFormat format, const void* source, size_t size,
                     size_t& destinationLength, size_t lineLength = 0);

        char* decode(EncodeFormat format, const char* source, size_t size,
                     size_t& destinationLength);

        std::string hash(HashFunction::Function function, Data* input);

        std::string hash(HashFunction::Function function, const char* input, uint64_t size);

        void hash(HashFunction::Function function, Data* input, HashFunction::Value& output);

        void hash(HashFunction::Function function, const char* input, uint64_t size,
                  HashFunction::Value& output);

        // clang-format off
        STRINGMAP_DECLARE(EncodeFormats, EncodeFormat,
            { "base64", ENCODE_BASE64 },
            { "hex",    ENCODE_HEX    }
        );

        STRINGMAP_DECLARE(ContainerTypes, ContainerType,
            { "data",   CONTAINER_DATA   },
            { "string", CONTAINER_STRING }
        );
        // clang-format on
    } // namespace data

    class DataModule : public Module
    {
      public:
        DataModule();

        virtual ~DataModule()
        {}

        DataView* newDataView(Data* data, size_t offset, size_t size) const;

        ByteData* newByteData(size_t size) const;

        ByteData* newByteData(const void* data, size_t size) const;

        ByteData* newByteData(void* data, size_t size, bool own) const;
    };
} // namespace love
