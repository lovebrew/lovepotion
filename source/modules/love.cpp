#include "runtime.h"
#include <unistd.h>

#include "version.h"

#include "audio.h"
#include "event.h"
#include "filesystem.h"
#include "graphics.h"
#include "system.h"
#include "timer.h"

#include <switch.h>

#include "gamepad.h"
#include "wrap_gamepad.h"

#include "love.h"

struct { const char * name; int (*fn)(lua_State *L); void (*close)(void); } modules[] = 
{
	//{ "audio",		Audio::Register,		Audio::Exit			},
	{ "event",		Event::Register,		NULL				},
	{ "filesystem",	Filesystem::Register,	NULL				},
	{ "graphics",	Graphics::Register,		Graphics::Exit		},
	{ "system",		System::Register,		NULL				},
	{ "timer",		Timer::Register,		NULL				},
	{ 0 }
};

int Love::Initialize(lua_State * L)
{
	int (*classes[])(lua_State *L) = 
	{
		initGamepadClass,
		NULL
	};

	for (int i = 0; classes[i]; i++) 
	{
		classes[i](L);
		lua_pop(L, 1);
	}

	luaL_Reg reg[] =
	{
		{ "getVersion",		GetVersion		},
		{ "enableConsole",	EnableConsole	},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	for (int i = 0; modules[i].name; i++)
	{
		modules[i].fn(L);
		lua_setfield(L, -2, modules[i].name);
	}

	return 1;
}

int Love::Scan(lua_State * L)
{
	//joycon/controllers
	hidScanInput();

	for (uint i = 0; i < controllers.size(); i++)
	{
		//BUTTON INPUT
		Love::GamepadPressed(L, controllers[i]);

		Love::GamepadReleased(L, controllers[i]);
		
		//LEFT STICK
		pair<string, float> leftStick = std::make_pair("nil", 0); 

		controllers[i]->ScanAxes(leftStick, JOYSTICK_LEFT, 0);
		Love::GamepadAxis(L, controllers[i], leftStick);

		controllers[i]->ScanAxes(leftStick, JOYSTICK_LEFT, 1);
		Love::GamepadAxis(L, controllers[i], leftStick);
		
		//RIGHT STICK
		pair<string, float> rightStick = std::make_pair("nil", 0); 

		controllers[i]->ScanAxes(rightStick, JOYSTICK_RIGHT, 0);
		Love::GamepadAxis(L, controllers[i], rightStick);

		controllers[i]->ScanAxes(rightStick, JOYSTICK_RIGHT, 1);
		Love::GamepadAxis(L, controllers[i], rightStick);
	}

	return 0;
}

void Love::GamepadPressed(lua_State * L, Gamepad * controller)
{
	string buttonDown = controllers[i]->ScanButtons(true);

	if (buttonDown != "nil")
	{
		love_getfield(L, "gamepadpressed");

		if (!lua_isnil(L, -1))
		{
			lua_pushuserdata(L, controllers[i], "Gamepad");
			lua_pushstring(L, buttonDown.c_str());
			lua_call(L, 2, 0);
		}
	}
}

void Love::GamepadReleased(lua_State * L, Gamepad * controller)
{
	string buttonUp = controllers[i]->ScanButtons(false);

	if (buttonUp != "nil")
	{
		love_getfield(L, "gamepadreleased");

		if (!lua_isnil(L, -1))
		{
			lua_pushuserdata(L, controllers[i], "Gamepad");
			lua_pushstring(L, buttonUp.c_str());
			lua_call(L, 2, 0);
		}
	}
}

void Love::GamepadAxis(lua_State * L, Gamepad * controller, pair<string, float> & data)
{
	if (data.second != 0)
	{
		love_getfield(L, "gamepadaxis");

		if (!lua_isnil(L, -1))
		{
			lua_pushuserdata(L, controller, "Gamepad");
			lua_pushstring(L, data.first.c_str());
			lua_pushnumber(L, data.second);
			lua_call(L, 3, 0);
		}
	}
}

int Love::GetVersion(lua_State * L)
{
	lua_pushnumber(L, Love::VERSION_MAJOR);
	lua_pushnumber(L, Love::VERSION_MINOR);
	lua_pushnumber(L, Love::VERSION_REVISION);
	lua_pushstring(L, Love::CODENAME.c_str());

	return 4;
}

int Love::EnableConsole(lua_State * L)
{
	lua_Debug info;
	int level = 0;

	bool canEnable = false;
	while (lua_getstack(L, level, &info))
	{
		lua_getinfo(L, "nfSl", &info);
		if (strncmp(info.short_src, "[string \"boot\"]", 15) == 0)
			canEnable = true;

		++level;
	}

	if (canEnable)
		Console::Initialize();

	return 0;
}

int Love::NoGame(lua_State * L)
{
	chdir("romfs:/");

	if (luaL_dofile(L, "main.lua"))
		Console::ThrowError(L);

	return 0;
}

void Love::Exit(lua_State * L)
{
	for (int i = 0; modules[i].close; i++)
		modules[i].close();

	lua_close(L);
}