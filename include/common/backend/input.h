/*
** input.h
** Handles internal input handling
*/

#pragma once

#if defined (_3DS)
    #include <3ds.h>
#elif defined (__SWITCH__)
    #include <switch.h>
#endif

#include <variant>
#include <list>
#include <array>
#include <unordered_map>

struct GamePadButton
{
    size_t which; //< Gamepad ID

    const char * name; //< Button Name
    int button; //< Button ID (0-index)
};

struct GamePadAxis
{
    size_t which; //< Gamepad ID

    const char * axis; //< Axis name
    float value; //< Axis value (0-1)
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

    LOVE_GAMEPADADDED,
    LOVE_GAMEPADREMOVED,

    LOVE_TOUCHPRESS,
    LOVE_TOUCHRELEASE,
    LOVE_TOUCHMOVED,

    LOVE_LOWMEMORY,

    LOVE_WINDOWEVENT,

    LOVE_FOCUS_GAINED,
    LOVE_FOCUS_LOST,

    LOVE_RESIZE,

    LOVE_QUIT
};

#if defined (_3DS)
    #define MAX_TOUCH 1
    typedef circlePosition StickPosition;
#elif defined (__SWITCH__)
    #define MAX_TOUCH 16
    typedef JoystickPosition StickPosition;
#endif

namespace Input
{
    bool PollEvent(LOVE_Event * event);

    /* Variables */

    inline std::variant<uint32_t, uint64_t> down = (uint32_t)0;
    inline std::variant<uint32_t, uint64_t> up   = (uint32_t)0;
    inline std::variant<uint32_t, uint64_t> held = (uint32_t)0;

    struct JoystickState
    {
        StickPosition left;
        StickPosition right;
    };

    inline std::array<touchPosition, MAX_TOUCH> touches;
    inline std::array<touchPosition, MAX_TOUCH> prevTouches;

    inline bool s_hysteresis = false;
    inline std::list<LOVE_Event> s_inputEvents;

    extern std::unordered_map<const char *, int> buttons;

    inline bool touchHeld = false;
    inline int prevTouchCount = 0;

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
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_WINDOWEVENT;
        e.subType = focus ? LOVE_FOCUS_GAINED : LOVE_FOCUS_LOST;
    }

    inline void SendQuit() {
        auto & e = s_inputEvents.emplace_back();
        e.type = LOVE_QUIT;
    }

    inline void SendLowMemory() {
        auto & e = s_inputEvents.emplace_back();
        e.type = LOVE_LOWMEMORY;
    }

    inline void SendResize(int width, int height) {
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_WINDOWEVENT;
        e.subType = LOVE_RESIZE;

        e.size.width = width;
        e.size.height = height;
    }

    #if defined (_3DS)
        void CheckFocus(APT_HookType type, void * param);
    #elif defined (__SWITCH__)
        void CheckFocus();
    #endif
};
