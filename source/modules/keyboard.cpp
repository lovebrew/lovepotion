#include "runtime.h"
#include "keyboard.h"

char buffer[255];
SwkbdState keyboard;
SwkbdButton button = SWKBD_BUTTON_NONE;

vector<string> BUTTONS = 
{
	"a", "b", "select", "start",
	"right", "left", "up", "down",
	"r", "l", "x", "y",
	"", "", "lz", "rz",
	"", "", "", "",
	"touch", "", "", "", "cstickright", 
	"cstickleft", "cstickup", "cstickdown",
	"cpadright", "cpadleft", "cpadup", "cpaddown"
};

u32 keyPress = 0;
u32 keyRelease = 0;
u32 keyHeld = 0;
touchPosition touch;

int lastTouch[2];
bool touchDown = false;

void Keyboard::Initialize()
{
	swkbdInit(&keyboard, SWKBD_TYPE_NORMAL, 2, -1);

	swkbdSetInitialText(&keyboard, buffer);
	swkbdSetHintText(&keyboard, "Input Text");

	swkbdSetFeatures(&keyboard, SWKBD_DEFAULT_QWERTY);
	swkbdSetValidation(&keyboard, SWKBD_NOTBLANK, 0, 0);
}

//Löve2D Functions

//love.keyboard.isDown
int Keyboard::IsDown(lua_State * L)
{
	keyHeld = hidKeysHeld();

	string key = string(luaL_checkstring(L, 1));

	bool isHeld = false;

	for (uint i = 0; i < BUTTONS.size(); i++)
	{
		if (keyPress & BIT(i) && BUTTONS[i] != "touch")
			isHeld = true;
	}

	lua_pushboolean(L, isHeld);

	return 1;
}

//love.keyboard.setTextInput
int Keyboard::SetTextInput(lua_State * L)
{
	bool openKeyboard = false;
	if (lua_isboolean(L, 1))
		openKeyboard = lua_toboolean(L, 1); 

	if (openKeyboard)
		button = swkbdInputText(&keyboard, buffer, sizeof(buffer));

	return 0;
}

//End Löve2D Functions

int Keyboard::Scan(lua_State * L)
{
	//Check software keyboard first!
	if (button != SWKBD_BUTTON_NONE)
	{
		lua_getfield(L, LUA_GLOBALSINDEX, "love");
		lua_getfield(L, -1, "textinput");
		lua_remove(L, -2);

		lua_pushstring(L, buffer);
		
		lua_call(L, 1, 0);

		memset(buffer, 0, 255);

		button = SWKBD_BUTTON_NONE;
	}

	hidScanInput();
	hidTouchRead(&touch);

	keyPress = hidKeysDown();
	keyRelease = hidKeysUp();

	for (uint i = 0; i < BUTTONS.size(); i++)
	{
		if (keyPress & BIT(i))
		{
			if (BUTTONS[i] != "touch")
			{
				lua_getfield(L, LUA_GLOBALSINDEX, "love");
				lua_getfield(L, -1, "keypressed");
				lua_remove(L, -2);

				if (!lua_isnil(L, -1))
				{
					lua_pushstring(L, BUTTONS[i].c_str());
					lua_call(L, 1, 0);
				}
			}
			else
			{
				lastTouch[0] = touch.px;
				lastTouch[1] = touch.py;

				touchDown = true;

				lua_getfield(L, LUA_GLOBALSINDEX, "love");
				lua_getfield(L, -1, "touchpressed");
				lua_remove(L, -2);

				if (!lua_isnil(L, -1))
				{
					lua_pushinteger(L, lastTouch[0]);
					lua_pushinteger(L, lastTouch[1]);
					lua_pushinteger(L, 1);
					lua_call(L, 3, 0);
				}
			}
		}
	}

	for (uint i = 0; i < BUTTONS.size(); i++)
	{
		if (keyRelease & BIT(i))
		{
			if (BUTTONS[i] != "touch")
			{
				lua_getfield(L, LUA_GLOBALSINDEX, "love");
				lua_getfield(L, -1, "keyreleased");
				lua_remove(L, -2);

				if (!lua_isnil(L, -1))
				{
					lua_pushstring(L, BUTTONS[i].c_str());
					lua_call(L, 1, 0);
				}
			}
			else
			{
				touchDown = false;
				
				lua_getfield(L, LUA_GLOBALSINDEX, "love");
				lua_getfield(L, -1, "touchreleased");
				lua_remove(L, -2);

				if (!lua_isnil(L, -1))
				{
					lua_pushinteger(L, lastTouch[0]);
					lua_pushinteger(L, lastTouch[1]);
					lua_pushinteger(L, 1);
					lua_call(L, 3, 0);
				}
			}
		}
	}

	return 0;
}

int Keyboard::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "scan",			Scan			},
		{ "isDown",			IsDown			},
		{ "setTextInput",	SetTextInput	},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	return 1;
}