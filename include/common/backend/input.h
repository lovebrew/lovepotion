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

struct Resize
{
    int width;
    int height;
};

struct LOVE_Event
{
    int type;
    int which;

    GamePadButton button;
    GamePadAxis axis;
    Finger touch;
    Resize size;
};

enum LOVE_EventType
{
    LOVE_GAMEPADAXIS,
    LOVE_GAMEPADDOWN,
    LOVE_GAMEPADUP,
    LOVE_TOUCHPRESS,
    LOVE_TOUCHRELEASE,
    LOVE_TOUCHMOVED,
    LOVE_LOWMEMORY,
    LOVE_FOCUS_GAINED,
    LOVE_FOCUS_LOST,
    LOVE_RESIZE,
    LOVE_QUIT
};

#if defined (_3DS)
    #define MAX_TOUCH 1
#elif defined (__SWITCH__)
    #define MAX_TOUCH 16
#endif

#include "objects/gamepad/gamepad.h"

namespace Input
{
    bool PollEvent(LOVE_Event * event);

    /* Variables */

    inline std::variant<u32, u64> down = (u32)0;
    inline std::variant<u32, u64> up   = (u32)0;
    inline std::variant<u32, u64> held = (u32)0;

    struct JoystickState
    {
        StickPosition left;
        StickPosition right;
    };

    inline std::array<touchPosition, MAX_TOUCH> touches;
    inline std::array<touchPosition, MAX_TOUCH> prevTouches;

    inline bool s_hysteresis = false;
    inline std::list<LOVE_Event> s_inputEvents;

    extern std::unordered_map<std::string, int> buttons;

    inline bool touchHeld = false;
    inline u32 prevTouchCount = 0;

    inline bool hasFocus  = true;
    inline bool prevFocus = false;

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

    inline void SendFocus(bool focus) {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = focus ? LOVE_FOCUS_GAINED : LOVE_FOCUS_LOST;
    }

    inline void SendQuit() {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_QUIT;
    }

    inline void SendLowMemory() {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_LOWMEMORY;
    }

    inline void SendResize(int width, int height) {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_RESIZE;
        e.size.width = width;
        e.size.height = height;
    }

    #if defined (_3DS)
        void CheckFocus(APT_HookType type, void * param);
    #elif defined (__SWITCH__)
        void CheckFocus();
    #endif
};
