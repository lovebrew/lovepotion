#pragma once

#include <common/object.hpp>

#include <stdint.h>

namespace love
{
    class Data : public Object
    {
      public:
        static Type type;

        virtual ~Data()
        {}

        virtual Data* Clone() const = 0;

        virtual void* GetData() const = 0;

        virtual size_t GetSize() const = 0;
    };
} // namespace love
