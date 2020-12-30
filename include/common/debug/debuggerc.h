#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

namespace love::common
{
    class Debugger
    {
        public:
            static Debugger & Instance() {
                static Debugger instance;
                return instance;
            }

            bool IsInited();

        protected:
            bool initialized;
            size_t count;

            Debugger();
    };
}