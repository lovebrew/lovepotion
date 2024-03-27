#pragma once

#include "common/Object.hpp"

#include <stddef.h>

#include <mutex>

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

        std::mutex* getMutex();

      private:
        std::mutex* mutex = nullptr;
        std::once_flag mutexCreated;
    };
} // namespace love
