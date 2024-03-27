#pragma once

#include "common/Object.hpp"
#include "common/Variant.hpp"

#include <condition_variable>
#include <mutex>

#include <queue>

namespace love
{
    class Channel : public Object
    {
      public:
        static Type type;

        Channel();

        virtual ~Channel()
        {}

        uint64_t push(const Variant& value);

        bool supply(const Variant& value);

        bool supply(const Variant& value, double timeout);

        bool pop(Variant* value);

        bool demand(Variant* value);

        bool demand(Variant* value, double timeout);

        bool peek(Variant* value);

        bool hasRead(uint64_t id);

        int getCount();

        void clear();

        void lockMutex();

        void unlockMutex();

        std::recursive_mutex& getMutex()
        {
            return this->mutex;
        }

      private:
        uint64_t sent;
        uint64_t received;

        std::recursive_mutex mutex;
        std::condition_variable_any condvar;

        std::queue<Variant> queue;
    };
} // namespace love
