#pragma once

#include "common/Object.hpp"
#include "modules/thread/threads.hpp"

#include <stddef.h>

namespace love
{
    class Data : public Object
    {
      public:
        static Type type;

        Data()
        {}

        virtual ~Data();

        virtual Data* clone() const = 0;

        virtual void* getData() const = 0;

        virtual size_t getSize() const = 0;

        Mutex& getMutex();

      private:
        Mutex mutex;
    };
} // namespace love
