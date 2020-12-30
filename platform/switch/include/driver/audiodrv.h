#pragma once

#include "common/driver/audiodrvc.h"
#include "modules/thread/types/mutex.h"

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
                    thread::Lock lock(this->mutex);
                    func(&this->driver);
                }
                catch(const std::exception & e)
                {
                    throw;
                }
            }

        private:
            thread::Mutex mutex;

            bool audioInitialized;
            AudioDriver driver;
    };
}