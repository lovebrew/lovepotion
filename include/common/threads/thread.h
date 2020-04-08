#pragma once

#if defined (_3DS)
    #include <3ds.h>
#elif defined (__SWITCH__)
    #include <switch.h>
#endif

namespace love::thread
{
    class Threadable
    {
        public:
            Threadable(void * arg);

        private:
            const void * args;
            Thread thread;
    };
}
