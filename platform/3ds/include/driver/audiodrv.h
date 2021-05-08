#pragma once

#include "driver/audiodrvc.h"
#include <3ds.h>

namespace love::driver
{
    class Audrv : public common::driver::Audrv
    {
      public:
        static Audrv& Instance()
        {
            static Audrv instance;
            return instance;
        }

        ~Audrv();

        Audrv(const Audrv&) = delete;

        Audrv(Audrv&&) = delete;

        Audrv operator=(const Audrv&) = delete;

        Audrv operator=(Audrv&&) = delete;

        LightEvent& GetEvent();

      private:
        Audrv();

        LightEvent ndspEvent;
    };
} // namespace love::driver
