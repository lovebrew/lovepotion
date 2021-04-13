#pragma once

#include "driver/audiodrvc.h"
#include <3ds.h>

namespace love::driver
{
    class Audrv : public common::driver::Audrv
    {
      public:
        Audrv();

        ~Audrv();

        Audrv(const Audrv&) = delete;

        Audrv(Audrv&&) = delete;

        Audrv operator=(const Audrv&) = delete;

        Audrv operator=(Audrv&&) = delete;

        LightEvent& GetEvent();

      private:
        LightEvent ndspEvent;
    };
} // namespace love::driver