#pragma once

#include "modules/thread/types/mutex.h"

namespace love::thread
{
    class Conditional
    {
      public:
        Conditional();
        ~Conditional();

        void Signal();
        void Broadcast();
        bool Wait(Mutex* mutex, s64 timeout = -1);

      private:
        CondVar condVar;
    };

    class ConditionalRef
    {
      public:
        ConditionalRef();
        ~ConditionalRef();

        operator Conditional*() const;
        Conditional* operator->() const;

      private:
        Conditional* conditional;
    };

    inline Conditional* NewConditional()
    {
        return new Conditional();
    }
} // namespace love::thread
