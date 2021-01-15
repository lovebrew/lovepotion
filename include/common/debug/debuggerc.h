#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

namespace love::common
{
    class Debugger
    {
        public:
            bool IsInited();

        protected:
            bool initialized;

            Debugger();
    };
}