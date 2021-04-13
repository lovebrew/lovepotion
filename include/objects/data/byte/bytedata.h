#pragma once

#include "common/data.h"
#include "common/exception.h"
namespace love
{
    class ByteData : public Data
    {
      public:
        static love::Type type;

        ByteData(size_t size);
        ByteData(const void* data, size_t size);
        ByteData(void* data, size_t size, bool own);
        ByteData(const ByteData& other);

        virtual ~ByteData();

        ByteData* Clone() const override;
        void* GetData() const override;
        size_t GetSize() const override;

      private:
        void Create();

        char* data;
        size_t size;
    };
} // namespace love
