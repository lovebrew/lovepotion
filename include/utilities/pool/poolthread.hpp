#pragma once

#include <utilities/pool/vibrations.hpp>

#include <utilities/threads/threadable.hpp>

namespace love
{
    class PoolThread : public Threadable
    {
      public:
        PoolThread(VibrationPool* pool);

        virtual ~PoolThread();

        void SetFinish();

        void ThreadFunction();

      private:
        VibrationPool* vibrations;
        std::atomic<bool> finish;
    };
} // namespace love
