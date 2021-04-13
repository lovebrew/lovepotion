#include <stdarg.h>
#include <stdio.h>
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
} // namespace love::common