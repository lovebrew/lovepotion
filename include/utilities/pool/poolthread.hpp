#pragma once

#include <utilities/pool/sources.hpp>
#include <utilities/pool/vibrations.hpp>

#include <utilities/threads/threadable.hpp>

namespace love
{
    class PoolThread : public Threadable
    {
      public:
        PoolThread(VibrationPool* pool);

        PoolThread(AudioPool* pool);

        virtual ~PoolThread();

        void SetFinish();

        void ThreadFunction();

      private:
        VibrationPool* vibrations;
        AudioPool* sources;

        std::atomic<bool> finish;
    };
} // namespace love
