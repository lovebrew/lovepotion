#include <unistd.h>

namespace love::common
{
    class Debugger
    {
      public:
        virtual bool Initialize() = 0;

      protected:
        bool initialized;

        Debugger();
    };
} // namespace love::common
