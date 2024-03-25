#pragma once

#include "common/Data.hpp"

#include <memory>

namespace love
{
    class ByteData : public Data
    {
      public:
        static Type type;

        ByteData(size_t size, bool clear = true);

        ByteData(const void* data, size_t size);

        ByteData(void* data, size_t size, bool owned);

        ByteData(const ByteData& other);

        virtual ~ByteData();

        ByteData* clone() const override;

        void* getData() const override;

        size_t getSize() const override;

      private:
        void create();

        char* data;
        size_t size;
    };
} // namespace love
