#pragma once

#include "common/data.hpp"

#include <memory>

namespace love
{
    class ByteData : public Data
    {
      public:
        static Type type;

        ByteData(size_t size, bool clear = true);

        ByteData(const void* data, size_t size);

        ByteData(void* data, size_t size, bool own);

        ByteData(const ByteData& other);

        ByteData* Clone() const override;

        void* GetData() const override;

        size_t GetSize() const override;

      private:
        void Create();

        std::unique_ptr<char[]> data;
        size_t size;
    };
} // namespace love
