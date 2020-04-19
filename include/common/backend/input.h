/*
** input.h
** Handles internal input handling
*/

#pragma once

struct GamePadButton
{
    std::string name;
    size_t which;
    int id;
};

struct GamePadAxis
{
    std::string axis;
    float value;
    int which;
};

struct Finger
{
    int64_t id;
    double x;
    double y;
    double dx;
    double dy;
    double pressure;
};

struct LOVE_Event
{
    int type;
    int which;

    GamePadButton button;
    GamePadAxis axis;
    Finger touch;
};

enum LOVE_EventType
{
    LOVE_GAMEPADAXIS,
    LOVE_GAMEPADDOWN,
    LOVE_GAMEPADUP,
    LOVE_TOUCHPRESS,
    LOVE_TOUCHRELEASE,
    LOVE_TOUCHMOVED
};

namespace Input
{
        bool PollEvent(LOVE_Event * event);

        /* Variables */

        inline std::variant<u32, u64> down = (u32)0;
        inline std::variant<u32, u64> up   = (u32)0;
        inline std::variant<u32, u64> held = (u32)0;

        inline StickPosition lastPosition[2] = { { 0, 0 } };
        inline std::array<int, 2> lastTouch =  { 0, 0 };

        extern std::unordered_map<std::string, int> buttons;

        inline bool touchHeld = false;

        /* Functions */

        template <typename T>
        inline T GetKeyDown() {
            return std::get<T>(down);
        }

        template <typename T>
        inline T GetKeyUp() {
            return std::get<T>(up);
        }

        template <typename T>
        inline T GetKeyHeld() {
            return std::get<T>(held);
        }
};
