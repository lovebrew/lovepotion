#pragma once

#include "common/driver/audrvc.h"
#include "modules/thread/types/mutex.h"
#include "pools/audiopool.h"

#include <switch.h>

#include <exception>

namespace love::driver
{
    class Audrv : public common::driver::Audrv
    {
        public:
            Audrv();

            ~Audrv();

            void Update();

            template <typename T>
            void LockFunction(const T & func)
            {
                try
                {
                    thread::Lock lock(mutex);
                    func(&driver);
                }
                catch(const std::exception & e)
                {
                    throw;
                }
            }

        private:
            thread::MutexRef mutex;
            AudioDriver driver;

            bool audioInitialized;
    };
}