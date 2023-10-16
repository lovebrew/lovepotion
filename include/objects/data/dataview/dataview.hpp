#pragma once

#include <common/data.hpp>
#include <common/strongreference.hpp>

namespace love
{
    class DataView : public Data
    {
      public:
        static Type type;

        DataView(Data* data, size_t offset, size_t size);

        DataView(const DataView& other);

        DataView* Clone() const override;

        void* GetData() const override;

        size_t GetSize() const override;

      private:
        StrongReference<Data> data;

        size_t offset;
        size_t size;
    };
} // namespace love
