#pragma once

#include "modules/thread/types/lock.h"

namespace love::common::driver
{
    class Audrv
    {
        public:
            Audrv();

            const bool IsInitialized();

        protected:
            bool initialized;
    };
}
