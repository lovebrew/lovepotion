#include "common/runtime.h"
#include "modules/event.h"

#include "objects/gamepad/gamepad.h"
#include "modules/joystick.h"

SDL_Event event;
int Event::PollEvent(lua_State * L)
{
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_JOYAXISMOTION:
		{
			Gamepad * controller = Joystick::GetJoystickFromID(event.jaxis.which);

			love_getfield(L, "gamepadaxis");
			if (!lua_isnil(L, -1))
			{
				lua_getglobal(L, "__controllers");
				lua_pushlightuserdata(L, controller);
				lua_gettable(L, -2);
				lua_remove(L, -2);

				lua_pushnumber(L, event.jaxis.axis + 1);

				float value = (float)event.jaxis.value / JOYSTICK_MAX;
				controller->ClampAxis(value);

				lua_pushnumber(L, value);
				lua_call(L, 3, 0);
			}
			break;
		}	
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		{	
			Gamepad * controller = Joystick::GetJoystickFromID(event.jbutton.which);
			love_getfield(L, (event.type == SDL_JOYBUTTONDOWN) ? "gamepadpressed" : "gamepadreleased");
			
			if (!lua_isnil(L, -1))
			{
				lua_getglobal(L, "__controllers");
				lua_pushlightuserdata(L, controller);
				lua_gettable(L, -2);
				lua_remove(L, -2);

				lua_pushstring(L, KEYS[event.jbutton.button].c_str());
				lua_call(L, 2, 0);
			}
			break;
		}
		/*case SDL_FINGERDOWN:
		case SDL_FINGERUP:
		{
			love_getfield(L, (event.type == SDL_FINGERDOWN) ? "touchpressed" : "touchreleased");

			if (!lua_isnil(L, -1))
			{
				lua_pushlightuserdata(L, (void *)event.tfinger.touchId);
				lua_pushnumber(L, event.tfinger.x * 1280);
				lua_pushnumber(L, event.tfinger.y * 720);
				lua_pushnumber(L, 0);
				lua_pushnumber(L, 0);
				lua_pushnumber(L, 1);

				lua_call(L, 6, 0);
			}
			break;
		}*/
		default:
			break;
	}

	return 0;
}

//Löve2D Functions

//love.event.quit
int Event::Quit(lua_State * L)
{
	LOVE_QUIT = true;

	return 0;
}

//End Löve2D Functions

int Event::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "quit", Quit },
		{ 0, 0}
	};

	luaL_newlib(L, reg);

	return 1;
}