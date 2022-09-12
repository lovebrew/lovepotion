#pragma once

#include <utilities/threads/thread.tcc>

namespace love
{
    template<>
    class Thread<Console::HAC> : public Thread<Console::ALL>
    {
      public:
        Thread(Threadable* threadable);

        virtual ~Thread()
        {
            if (this->thread.joinable())
                this->thread.join();
        }

        bool Start();

        int Runner();
    };
} // namespace love
