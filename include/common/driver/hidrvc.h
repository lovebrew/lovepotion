#pragma once

#if defined (_3DS)
    #include <3ds.h>
#else
    #include <switch.h>
#endif

#include <list>
#include <array>

#include "modules/thread/types/mutex.h"
#include "modules/thread/types/lock.h"

namespace love::common::driver
{
    class Hidrv
    {
        public:
            struct ButtonMapping
            {
                const char * name;
                uint32_t key;
                int index;
            };

            struct GamePadButton
            {
                size_t which; //< Gamepad ID

                const char * name; //< Button Name
                int button; //< Button ID (0-index)
            };

            struct GamePadAxis
            {
                size_t which; //< Gamepad ID

                size_t number;
                const char * axis; //< Axis name
                float value; //< Axis value (0-1)
            };

            struct GamePadStatus
            {
                size_t which; //< Gamepad ID

                bool connected;
            };

            struct Finger
            {
                int64_t id; //< Touch ID

                double x;
                double y;
                double dx;
                double dy;
                double pressure;
            };

            struct Resize
            {
                int width;
                int height;
            };

            struct LOVE_Event
            {
                uint8_t type;
                uint8_t subType;

                GamePadStatus padStatus;
                GamePadButton button;
                GamePadAxis axis;

                Finger touch;
                Resize size;
            };

            enum EventType
            {
                TYPE_GAMEPADAXIS,
                TYPE_GAMEPADDOWN,
                TYPE_GAMEPADUP,

                TYPE_GAMEPADADDED,
                TYPE_GAMEPADREMOVED,

                TYPE_TOUCHPRESS,
                TYPE_TOUCHRELEASE,
                TYPE_TOUCHMOVED,

                TYPE_LOWMEMORY,

                TYPE_WINDOWEVENT,

                TYPE_FOCUS_GAINED,
                TYPE_FOCUS_LOST,

                TYPE_RESIZE,

                TYPE_QUIT
            };

            Hidrv();

            uint64_t GetButtonPressed();

            uint64_t GetButtonReleased();

            uint64_t GetButtonHeld();

            virtual bool Poll(LOVE_Event * event) = 0;

            virtual bool IsDown(size_t button) = 0;

            void SendFocus(bool focus);

            void SendQuit();

            void SendLowMemory();

            void SendResize(int width, int height);

        protected:
            bool hysteresis;
            std::list<LOVE_Event> events;

            love::thread::Mutex mutex;

            struct ButtonState
            {
                uint64_t oldPressed;
                uint64_t pressed;

                uint64_t oldReleased;
                uint64_t released;

                uint64_t oldHeld;
                uint64_t held;
            } buttonStates;

            virtual void Lock() {};

            virtual void Unlock() {};

            template <typename T>
            void LockFunction(const T & func)
            {
                this->Lock();
                func();
                this->Unlock();
            }

    };
}