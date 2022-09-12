#pragma once

#include <common/strongreference.hpp>

#include <memory>

#include <utilities/stream/stream.hpp>

namespace love
{
    class DataStream : public Stream
    {
      public:
        static Type type;

        DataStream(Data* data);

        DataStream* Clone() override;

        bool IsReadable() const override;

        bool IsWritable() const override;

        bool IsSeekable() const override;

        int64_t Read(void* data, int64_t size) override;

        bool Write(const void* data, int64_t size) override;

        bool Flush() override;

        int64_t GetSize() override;

        bool Seek(int64_t position, SeekOrigin origin = SeekOrigin::ORIGIN_BEGIN) override;

        int64_t Tell() override;

      private:
        DataStream(const DataStream& other);

        StrongReference<Data> data;

        struct DataStreamMemory
        {
            std::unique_ptr<const uint8_t[]> readOnly;
            std::unique_ptr<uint8_t[]> writeable;

            size_t offset;
            size_t size;
        } memory;
    };
} // namespace love
