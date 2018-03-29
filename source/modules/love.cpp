#include "common/runtime.h"
#include <unistd.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "common/version.h"

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#include "modules/audio.h"
#include "modules/event.h"
#include "modules/filesystem.h"
#include "modules/graphics.h"
#include "modules/joystick.h"
#include "modules/mod_math.h"
#include "modules/system.h"
#include "modules/timer.h"
#include "modules/touch.h"

#include "objects/file/wrap_file.h"
#include "objects/font/wrap_font.h"
#include "objects/source/wrap_source.h"
#include "objects/image/wrap_image.h"
#include "objects/quad/wrap_quad.h"

#include "modules/love.h"

struct { const char * name; int (*fn)(lua_State *L); void (*close)(void); } modules[] = 
{
	{ "audio",		Audio::Register,		Audio::Exit			},
	{ "event",		Event::Register,		NULL				},
	{ "filesystem",	Filesystem::Register,	NULL				},
	{ "graphics",	Graphics::Register,		NULL				},
	{ "joystick",	Joystick::Register,		NULL				},
	{ "math",		Math::Register,			NULL				},
	{ "system",		System::Register,		System::Exit		},
	{ "timer",		Timer::Register,		NULL				},
	{ "touch",		Touch::Register,		NULL				},
	{ 0 }
};

int Love::Initialize(lua_State * L)
{
	int (*classes[])(lua_State *L) = 
	{
		initFileClass,
		initGamepadClass,
		initSourceClass,
		initImageClass,
		initQuadClass,
		initFontClass,
		NULL
	};

	for (int i = 0; classes[i]; i++) 
	{
		classes[i](L);
		lua_pop(L, 1);
	}

	luaL_Reg reg[] =
	{
		{ "scan",			Scan			},
		{ "run",			Run				},
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

	lua_newtable(L);
	lua_setglobal(L, "__controllers");

	return 1;
}

vector<vector<u32>> touches;
u32 lastTouchCount = 0;
bool touchDown = false;

bool Love::IsRunning()
{
	return (ERROR == false && LOVE_QUIT == false);
}

SDL_Event event;
int Love::Scan(lua_State * L)
{
	//joycon/controllers
	//hidScanInput();

	Event::JoystickEvent(L, event);
	
	/*u64 touchDown = hidKeysDown(CONTROLLER_HANDHELD);
	u64 touchUp = hidKeysUp(CONTROLLER_HANDHELD);
	u64 touchHeld = hidKeysHeld(CONTROLLER_HANDHELD);

	if (touchDown & KEY_TOUCH)
		Love::TouchPressed(L);

	if (touchHeld & KEY_TOUCH)
		Love::TouchMoved(L);

	u32 touchCount = hidTouchCount();
	if (touchCount != lastTouchCount)
		lastTouchCount = touchCount;

	if (touchUp & KEY_TOUCH)
		Love::TouchReleased(L);
	*/
	return 0;
}

int Love::Run(lua_State * L)
{
	Love::Scan(L);

	if (luaL_dostring(L, LOVE_TIMER_STEP))
		throw Exception(L);

	if (luaL_dostring(L, LOVE_UPDATE))
		throw Exception(L);

	if (!Console::IsInitialized())
		luaL_dostring(L, LOVE_CLEAR);
	
	if (luaL_dostring(L, LOVE_DRAW))
		throw Exception(L);

	luaL_dostring(L, LOVE_PRESENT);

	Timer::Tick();

	return 0;
}

/*void Love::GamepadPressed(lua_State * L, Gamepad * controller, SDL_Event event)
{
	Event::JoystickEvent(&event, controller);

	if (buttonDown != "nil")
	{
		love_getfield(L, "gamepadpressed");

		if (!lua_isnil(L, -1))
		{
			lua_getglobal(L, "__controllers");
			lua_pushlightuserdata(L, controller);
			lua_gettable(L, -2);
			lua_remove(L, -2);

			lua_pushstring(L, buttonDown.c_str());
			lua_call(L, 2, 0);
		}
	}
}*/

/*void Love::GamepadReleased(lua_State * L, Gamepad * controller)
{
	string buttonUp = controller->ScanButtons(false);

	if (buttonUp != "nil")
	{
		love_getfield(L, "gamepadreleased");

		if (!lua_isnil(L, -1))
		{
			lua_getglobal(L, "__controllers");
			lua_pushlightuserdata(L, controller);
			lua_gettable(L, -2);
			lua_remove(L, -2);

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
			lua_getglobal(L, "__controllers");
			lua_pushlightuserdata(L, controller);
			lua_gettable(L, -2);
			lua_remove(L, -2);

			lua_pushstring(L, data.first.c_str());
			lua_pushnumber(L, data.second);
			lua_call(L, 3, 0);
		}
	}
}*/

void Love::TouchPressed(lua_State * L)
{
	touchPosition touch;

	touchDown = false;
	touches.clear();

	for (u32 id = 0; id < hidTouchCount(); id++)
	{
		hidTouchRead(&touch, id);

		u32 x = touch.px;
		u32 y = touch.py;

		love_getfield(L, "touchpressed");

		if (!lua_isnil(L, -1))
		{
			lua_pushlightuserdata(L, &id);
			lua_pushinteger(L, x);
			lua_pushinteger(L, y);
			lua_pushinteger(L, 0);
			lua_pushinteger(L, 0);
			lua_pushinteger(L, 1);

			lua_call(L, 6, 0);
		}

		touches.push_back({id, x, y});
		touchDown = true;
	}
}

void Love::TouchMoved(lua_State * L)
{

	if (!touchDown)
		return;

	touchPosition touch;

	for (u32 i = 0; i < hidTouchCount(); i++)
	{
		try
		{
			hidTouchRead(&touch, touches[i][0]);

			u32 x = touch.px;
			u32 y = touch.py;
			
			s32 dx = touches[i][1] - x;
			s32 dy = touches[i][2] - y;

			love_getfield(L, "touchmoved");

			if (!lua_isnil(L, -1))
			{
				if (x != touches[i][1] && y != touches[i][2])
				{
					lua_pushlightuserdata(L, &touches[i][0]);
					lua_pushinteger(L, touches[i][1]);
					lua_pushinteger(L, touches[i][2]);
					lua_pushinteger(L, dx);
					lua_pushinteger(L, dy);
					lua_pushinteger(L, 1);

					lua_call(L, 6, 0);
				}

				touches[i] = {i, x, y};
			}
		}
		catch (std::exception & e)
		{
			printf("\n");
		}
	}
}

void Love::TouchReleased(lua_State * L)
{
	if (touches.size() > 0)
	{
		love_getfield(L, "touchreleased");

		if (!lua_isnil(L, -1))
		{
			if (touches.size() == 1)
			{
				u32 id = touches[0][0];
				u32 x = touches[id][1];
				u32 y = touches[id][2];

				lua_pushlightuserdata(L, &id);
				lua_pushinteger(L, x);
				lua_pushinteger(L, y);
				lua_pushinteger(L, 0);
				lua_pushinteger(L, 0);
				lua_pushinteger(L, 1);

				lua_call(L, 6, 0);
			}
			else
			{
				touchPosition position;

				for (u32 id = 0; id < touches.size(); id++)
				{
					for (u32 touch = 0; touch < hidTouchCount(); touch++)
					{
						hidTouchRead(&position, touch);

						printf("%d, %d -> %d, %d", touches[id][1], touches[id][2], position.px, position.py);
					}
					/*u32 x = touches[id][1];
					u32 y = touches[id][2];

					printf("[RELEASE] Touch %d: %d, %d\n", id, x, y);

					lua_pushlightuserdata(L, &id);
					lua_pushinteger(L, x);
					lua_pushinteger(L, y);
					lua_pushinteger(L, 0);
					lua_pushinteger(L, 0);
					lua_pushinteger(L, 1);

					lua_call(L, 6, 0);*/
				}
			}
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
	Console::Initialize(L, false);

	return 0;
}

int Love::NoGame(lua_State * L)
{
	chdir("romfs:/");

	if (luaL_dofile(L, "main.lua"))
		throw Exception(L);

	return 0;
}

void Love::Exit(lua_State * L)
{
	controllers.clear();

	for (int i = 0; modules[i].close; i++)
		modules[i].close();

	lua_close(L);

	Graphics::Exit();
}
