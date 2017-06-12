#include "common/runtime.h"
#include "crendertarget.h"

#include "graphics.h"
#include "wrap_image.h"
#include "basic_shader_shbin.h"

love::Graphics * love::Graphics::instance = 0;

using love::Graphics;
using love::CRenderTarget;

int sLocProjection = -1;
DVLB_s* dvlb = nullptr;
gfxScreen_t Graphics::currentScreen = GFX_TOP;
CRenderTarget * Graphics::bottomTarget = nullptr;

Graphics::Graphics() {}

int Graphics::GetWidth(lua_State * L)
{
	int width = 400;
	if (currentScreen == GFX_BOTTOM)
		width = 320;

	lua_pushnumber(L, width);

	return 1;
}

int Graphics::GetHeight(lua_State * L)
{
	lua_pushnumber(L, 240);

	return 1;
}

int Graphics::SetScreen(lua_State * L)
{
	const char * screen = luaL_checkstring(L, 1);

	if (strncmp(screen, "top", 3) == 0)
		currentScreen = GFX_TOP;
	else if (strncmp(screen, "bottom", 6) == 0)
		currentScreen = GFX_BOTTOM;

	return 0;
}

gfxScreen_t Graphics::GetScreen()
{
	return currentScreen;
}

C3D_RenderTarget * Graphics::GetRenderTarget(unsigned int i)
{
	C3D_RenderTarget * ret;
	switch(i)
	{
		case 0:
			ret = nullptr;
			break;
		case 1:
			ret = nullptr;
			break;
		default:
			ret = bottomTarget->target;
			break;
	}

	return ret;
}

void Graphics::InitRenderTargets()
{
	this->bottomTarget = new CRenderTarget(320, 240);
	C3D_RenderTargetSetOutput(this->bottomTarget->target, GFX_BOTTOM, GFX_LEFT, 0x1000);

	shaderProgram_s shader;

	// shader and company
    dvlb = DVLB_ParseFile((u32*)basic_shader_shbin, basic_shader_shbin_size);
    shaderProgramInit(&shader);
    shaderProgramSetVsh(&shader, &dvlb->DVLE[0]);
    C3D_BindProgram(&shader);

    sLocProjection = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");
    
    // set up mode defaults
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA); // premult
    C3D_DepthTest(false, GPU_GEQUAL, GPU_WRITE_ALL); // hmm.
    C3D_CullFace(GPU_CULL_NONE);
}

int graphicsInit(lua_State * L)
{	
	gfxInitDefault();
	gfxSet3D(false);

	C3D_Init(0x80000 * 8);

	love::Graphics::Instance()->InitRenderTargets();

	luaL_Reg reg[] = 
	{
		{ "getWidth",	love::Graphics::GetWidth	},
		{ "getHeight",	love::Graphics::GetHeight	},
		{ "setScreen",	love::Graphics::SetScreen	},
		{ "newImage",	imageNew					},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}