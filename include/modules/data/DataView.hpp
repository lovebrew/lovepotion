#pragma once

#include "common/Data.hpp"
#include "common/StrongRef.hpp"

namespace love
{
    class DataView : public Data
    {
      public:
        static Type type;

        DataView(Data* data, size_t offset, size_t size);

        DataView(const DataView& other);

        virtual ~DataView();

        DataView* clone() const override;

        void* getData() const override;

        size_t getSize() const override;

      private:
        StrongRef<Data> data;
        size_t offset;
        size_t size;
    };
} // namespace love
