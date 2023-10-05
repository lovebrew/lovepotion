#pragma once

#include <3ds.h>

#include <utilities/driver/hid.tcc>

namespace love
{
    template<>
    class HID<Console::CTR> : public HID<Console::ALL>
    {
      public:
        static HID& Instance()
        {
            static HID instance;
            return instance;
        }

        ~HID();

        virtual void _Poll() override;

      private:
        HID();

        struct
        {
            touchPosition current;
            touchPosition previous;
        } touchState;
    };
} // namespace love
