#pragma once

#include "common/Stream.hpp"
#include "common/StrongRef.hpp"

namespace love
{
    class DataStream : public Stream
    {
      public:
        static Type type;

        DataStream(Data* data);

        virtual ~DataStream();

        virtual DataStream* clone() const override;

        virtual bool isReadable() const override;

        virtual bool isWritable() const override;

        virtual bool isSeekable() const override;

        virtual int64_t read(void* data, int64_t size) override;

        virtual bool write(const void* data, int64_t size) override;

        virtual bool flush() override;

        virtual int64_t getSize() override;

        virtual bool seek(int64_t pos, SeekOrigin origin = SEEKORIGIN_BEGIN) override;

        virtual int64_t tell() override;

      private:
        DataStream(const DataStream& other);

        StrongRef<Data> data;

        const uint8_t* memory;
        uint8_t* writableMemory;

        size_t offset;
        size_t size;
    };
} // namespace love
