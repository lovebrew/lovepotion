#pragma once

#include "objects/object.h"
#include <string>

namespace love
{
    class Thread;

    class Threadable : public love::Object
    {
      public:
        static love::Type type;

        Threadable();
        virtual ~Threadable();

        virtual void ThreadFunction() = 0;

        bool Start();

        void Wait();

        bool IsRunning() const;

        const char* GetThreadName() const;

      private:
        friend class love::Thread;

      protected:
        love::Thread* owner;
        std::string threadName;
    };
} // namespace love
