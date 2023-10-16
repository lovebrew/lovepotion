#pragma once

#include <common/object.hpp>

#include <utilities/threads/thread.hpp>

namespace love
{
    class Threadable : public Object
    {
      public:
        static Type type;

        Threadable();

        virtual ~Threadable();

        virtual void ThreadFunction() = 0;

        bool Start();

        void Wait();

        bool IsRunning() const;

        const char* GetThreadName() const;

      protected:
        love::Thread* owner;
        std::string name;
    };
} // namespace love
