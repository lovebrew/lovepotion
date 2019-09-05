/*
** modules/joystick.h
** @brief    : Handles (basic) joystick stuff.
** @ref        : See wrap_gamepad.cpp for advanced use.
*/

#pragma once

class Joystick
{
    public:
        static void Initialize(lua_State * L); //should set up joysticks

        static Gamepad * GetJoystickFromID(uint id);

        static int Register(lua_State * L);
        
        static void AddJoystick(Gamepad * gamepad);
        
        static void RemoveJoystick(uint id);

        static std::vector<Gamepad *> GetJoysticks();

    private:
        static inline std::vector<Gamepad *> controllers;

        //Löve2D Functions

        static int GetJoysticks(lua_State * L);

        static int GetJoystickCount(lua_State * L);

        //End Löve2D Functions
};
