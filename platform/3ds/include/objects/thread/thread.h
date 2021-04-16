#pragma once

#include "thread/threadc.h"

namespace love
{
    class Thread : public common::Thread
    {
      public:
        Thread(Threadable* t);

        /* Detach the thread */
        virtual ~Thread();

        virtual bool Start() override;

        /*
        ** In SDL, SDL_WaitThread will wait
        ** for the thread to be finished and then
        ** SDL frees the pointer to the thread
        */
        virtual void Wait() override;
    };

    inline Thread* newThread(Threadable* t)
    {
        return new Thread(t);
    }
} // namespace love
