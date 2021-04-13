#pragma once

#include "common/variant.h"
#include "objects/object.h"

#include "modules/thread/types/conditional.h"
#include "modules/thread/types/lock.h"
#include "modules/thread/types/mutex.h"

#include <queue>

namespace love
{
    class Channel : public Object
    {
        // for the Wrapper
        friend int Wrap_Channel_PerformAtomic(lua_State*);

      public:
        static love::Type type;

        Channel();

        virtual ~Channel();

        uint64_t Push(const Variant& variant);

        // Blocking push
        bool Supply(const Variant& variant);

        bool Supply(const Variant& variant, double timeout);

        bool Pop(Variant* variant);

        // Blocking pop
        bool Demand(Variant* variant);

        bool Demand(Variant* variant, double timeout);

        bool Peek(Variant* variant);

        int GetCount() const;

        bool HasRead(uint64_t id) const;

        void Clear();

      private:
        void LockMutex();
        void UnlockMutex();
        bool _Pop(Variant* variant);
        uint64_t _Push(const Variant& variant);

        thread::MutexRef mutex;
        thread::ConditionalRef condition;
        std::queue<Variant> queue;

        uint64_t sent;
        uint64_t received;
    };

    int Wrap_Channel_PerformAtomic(lua_State*);
} // namespace love
