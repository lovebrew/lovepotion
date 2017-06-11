#include "common/runtime.h"
#include "softwarekeyboard.h"
#include "keyboard.h"

using love::Keyboard;

char BUTTONS[32][32] = {
	"a", "b", "select", "start",
	"right", "left", "up", "down",
	"rbutton", "lbutton", "x", "y",
	"", "", "lzbutton", "rzbutton",
	"", "", "", "",
	"touch", "", "", "",
	"cstickright", "cstickleft", "cstickup", "cstickdown",
	"cpadright", "cpadleft", "cpadup", "cpaddown"
};

bool Keyboard::TEXT_INPUT = false;
love::SoftwareKeyboard * love::Keyboard::keyboard;

int Keyboard::IsDown(lua_State * L)
{
	const char * key = luaL_checkstring(L, 1);
	u32 keyHeld = hidKeysHeld();
	bool keyDown = false;

	for (int i = 0; i < 32; i++)
	{
		if (keyHeld & BIT(i))
		{
			if (strcmp(BUTTONS[i], "touch") != 0)
			{
				keyDown = true;
			}
		}
	}

	lua_pushboolean(L, keyDown);

	return 1;
}

int Keyboard::SetTextInput(lua_State * L)
{
	if (lua_isboolean(L, 1))
		TEXT_INPUT = lua_toboolean(L, 1);
	
	if (TEXT_INPUT)
	{
		keyboard = new love::SoftwareKeyboard();
	}
	else
	{
		if (keyboard == nullptr)
			return 0;
		else
		{			
			lua_getfield(L, LUA_GLOBALSINDEX, "love");
			lua_getfield(L, -1, "textinput");
			lua_remove(L, -2);

			const char * text = keyboard->GetText();
			lua_pushstring(L, text);
			
			lua_call(L, 1, 0);

			delete keyboard;

			return 1;
		}
	}

	return 0;
}

int Keyboard::HasTextInput(lua_State * L)
{
	lua_pushboolean(L, TEXT_INPUT);

	return 1;
}

int keyboardInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "isDown",			love::Keyboard::IsDown			},
		{ "setTextInput",	love::Keyboard::SetTextInput	},
		{ "hasTextInput",	love::Keyboard::HasTextInput	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}