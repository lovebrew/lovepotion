#include "common/runtime.h"
#include "crendertarget.h"

#include "graphics.h"
#include "wrap_image.h"
#include "basic_shader_shbin.h"

love::Graphics * love::Graphics::instance = 0;

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

using love::Graphics;
using love::CRenderTarget;

int projection_desc = -1;
int transform_desc = -1;
int use_transform_desc = -1;

DVLB_s * dvlb = nullptr;
shaderProgram_s shader;

gfxScreen_t Graphics::currentScreen = GFX_TOP;
CRenderTarget * Graphics::bottomTarget = nullptr;
int Graphics::render = 0;

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

typedef struct { float x, y, z; } vertex;

static const vertex vertex_list[] =
{
	{ 200.0f, 200.0f, 0.5f },
	{ 100.0f, 40.0f, 0.5f },
	{ 300.0f, 40.0f, 0.5f },
};

#define vertex_list_count (sizeof(vertex_list)/sizeof(vertex_list[0]))

static void * vbo_data;

int Graphics::Line(lua_State * L)
{	
	// Configure the first fragment shading substage to just pass through the vertex color
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

	// Configure attributes for use with the vertex shader
	C3D_AttrInfo * attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddFixed(attrInfo, 1); // v1=color

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 1, 0x0);

	C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);
}

gfxScreen_t Graphics::GetScreen()
{
	return currentScreen;
}

CRenderTarget * Graphics::GetRenderTarget(unsigned int i)
{
	CRenderTarget * ret;
	switch(i)
	{
		case 0:
			ret = nullptr;
			break;
		case 1:
			ret = bottomTarget;
			break;
		default:
			ret = nullptr;
			break;
	}

	return ret;
}

void Graphics::InitRenderTargets()
{	
	bottomTarget = new CRenderTarget(320, 240);
	C3D_RenderTargetSetOutput(bottomTarget->target, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	Mtx_OrthoTilt(&bottomTarget->projection, 0.0f, 320, 240, 0.0f, 0.0f, 1.0f, true);

	// shader and company
	dvlb = DVLB_ParseFile((u32 *)basic_shader_shbin, basic_shader_shbin_size);
	shaderProgramInit(&shader);
	shaderProgramSetVsh(&shader, &dvlb->DVLE[0]);
	C3D_BindProgram(&shader);

	C3D_CullFace(GPU_CULL_NONE);
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

	projection_desc = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");
	transform_desc = shaderInstanceGetUniformLocation(shader.vertexShader, "transform");
	use_transform_desc = shaderInstanceGetUniformLocation(shader.vertexShader, "useTransform");

	C3D_BoolUnifSet(GPU_VERTEX_SHADER, use_transform_desc, false);
	C3D_FixedAttribSet(1, 1.0, 1.0, 1.0, 1.0);

	// set up mode defaults
	C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA); // premult

	//TRIANGLE TESTS
	// Create the VBO (vertex buffer object)
	vbo_data = linearAlloc(sizeof(vertex_list));
	memcpy(vbo_data, vertex_list, sizeof(vertex_list));
}

void Graphics::Render(gfxScreen_t screen)
{
	CRenderTarget * ctarget = GetRenderTarget(screen);

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW); //SYNC_DRAW

	C3D_FrameDrawOn(bottomTarget->target);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, projection_desc, &bottomTarget->projection);
}

void Graphics::SwapBuffers()
{
	C3D_FrameEnd(0);
}

int graphicsInit(lua_State * L)
{	
	gfxInitDefault();
	gfxSet3D(false);

	C3D_Init(0x80000 * 8);

	love::Graphics::Instance()->InitRenderTargets();
	
	luaL_Reg reg[] = 
	{
		{ "line",		love::Graphics::Line		},
		{ "getWidth",	love::Graphics::GetWidth	},
		{ "getHeight",	love::Graphics::GetHeight	},
		{ "setScreen",	love::Graphics::SetScreen	},
		{ "newImage",	imageNew					},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}