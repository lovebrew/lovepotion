#pragma once

#include "common/data.h"

namespace love
{
    class DataView : public Data
    {
      public:
        static love::Type type;

        DataView(Data* data, size_t offset, size_t size);
        DataView(const DataView& other);

        virtual ~DataView();

        DataView* Clone() const override;
        void* GetData() const override;
        size_t GetSize() const override;

      private:
        StrongReference<Data> data;

        size_t offset;
        size_t size;
    };
} // namespace love
