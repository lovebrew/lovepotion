#pragma once

#include <utilities/threads/threadable.hpp>

namespace love
{
    class SwkbdThread : public Threadable
    {
      public:
        virtual ~SwkbdThread();

        void SetFinish();

        void ThreadFunction();

      private:
        std::atomic<bool> finish;
    };
} // namespace love
