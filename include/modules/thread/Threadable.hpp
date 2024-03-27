#pragma once

#include "common/Object.hpp"

#include "modules/thread/Thread.hpp"

namespace love
{
    class Threadable : public Object
    {
      public:
        static Type type;

        Threadable();

        virtual ~Threadable();

        virtual void run() = 0;

        bool start();

        void wait();

        bool isRunning();

        const char* getThreadName() const;

      protected:
        Thread* owner;
        std::string threadName;
    };
} // namespace love
