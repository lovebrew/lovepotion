#include <string.h>
#include "sf2d.h"
#include "sf2d_private.h"
#include "shader_vsh_shbin.h"


static int sf2d_initialized = 0;
static u32 clear_color = 0;
static u32 *gpu_cmd = NULL;
//GPU init variables
static int gpu_cmd_size = 0;
// Temporary memory pool
static void *pool_addr = NULL;
static u32 pool_index = 0;
static u32 pool_size = 0;
//GPU framebuffer address
static u32 *gpu_fb_addr = NULL;
//GPU depth buffer address
static u32 *gpu_depth_fb_addr = NULL;
//VBlank wait
static int vblank_wait = 1;
//FPS calculation
static float current_fps = 0.0f;
static unsigned int frames = 0;
static u64 last_time = 0;
//Current screen/side
static gfxScreen_t cur_screen = GFX_TOP;
static gfx3dSide_t cur_side = GFX_LEFT;
//Shader stuff
static DVLB_s *dvlb = NULL;
static shaderProgram_s shader;
static u32 projection_desc = -1;
//Matrix
static float ortho_matrix_top[4*4];
static float ortho_matrix_bot[4*4];
//Rendertarget things
static sf2d_rendertarget * currentRenderTarget = NULL;
static void * targetDepthBuffer;
static int targetDepthBufferLen = 0;
//Apt hook cookie
static aptHookCookie apt_hook_cookie;
//Functions
static void apt_hook_func(APT_HookType hook, void *param);
static void reset_gpu_apt_resume();

int sf2d_init()
{
	return sf2d_init_advanced(
		SF2D_GPUCMD_DEFAULT_SIZE,
		SF2D_TEMPPOOL_DEFAULT_SIZE);
}

int sf2d_init_advanced(int gpucmd_size, int temppool_size)
{
	if (sf2d_initialized) return 0;

	gpu_fb_addr       = vramMemAlign(400*240*8, 0x100);
	gpu_depth_fb_addr = vramMemAlign(400*240*8, 0x100);
	gpu_cmd           = linearAlloc(gpucmd_size * 4);
	pool_addr         = linearAlloc(temppool_size);
	pool_size         = temppool_size;
	gpu_cmd_size      = gpucmd_size;

	gfxInitDefault();
	GPU_Init(NULL);
	gfxSet3D(false);
	GPU_Reset(NULL, gpu_cmd, gpucmd_size);

	//Setup the shader
	dvlb = DVLB_ParseFile((u32 *)shader_vsh_shbin, shader_vsh_shbin_size);
	shaderProgramInit(&shader);
	shaderProgramSetVsh(&shader, &dvlb->DVLE[0]);

	//Get shader uniform descriptors
	projection_desc = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");

	shaderProgramUse(&shader);

	matrix_init_orthographic(ortho_matrix_top, 0.0f, 400.0f, 0.0f, 240.0f, 0.0f, 1.0f);
	matrix_init_orthographic(ortho_matrix_bot, 0.0f, 320.0f, 0.0f, 240.0f, 0.0f, 1.0f);
	matrix_gpu_set_uniform(ortho_matrix_top, projection_desc);

	//Register the apt callback hook
	aptHook(&apt_hook_cookie, apt_hook_func, NULL);

	vblank_wait = 1;
	current_fps = 0.0f;
	frames = 0;
	last_time = osGetTime();

	cur_screen = GFX_TOP;
	cur_side = GFX_LEFT;

	GPUCMD_Finalize();
	GPUCMD_FlushAndRun();
	gspWaitForP3D();

	sf2d_pool_reset();

	sf2d_initialized = 1;

	return 1;
}

int sf2d_fini()
{
	if (!sf2d_initialized) return 0;

	aptUnhook(&apt_hook_cookie);

	gfxExit();
	shaderProgramFree(&shader);
	DVLB_Free(dvlb);

	linearFree(pool_addr);
	linearFree(gpu_cmd);
	vramFree(gpu_fb_addr);
	vramFree(gpu_depth_fb_addr);
	linearFree(targetDepthBuffer);

	sf2d_initialized = 0;

	return 1;
}

void sf2d_set_3D(int enable)
{
	gfxSet3D(enable);
}

void sf2d_start_frame(gfxScreen_t screen, gfx3dSide_t side)
{
	sf2d_pool_reset();
	GPUCMD_SetBufferOffset(0);

	// Only upload the uniform if the screen changes
	if (screen != cur_screen) {
		if (screen == GFX_TOP) {
			matrix_gpu_set_uniform(ortho_matrix_top, projection_desc);
		} else {
			matrix_gpu_set_uniform(ortho_matrix_bot, projection_desc);
		}
		cur_screen = screen;
	}

	int screen_w;
	if (screen == GFX_TOP) {
		screen_w = 400;
		cur_side = side;
	} else {
		screen_w = 320;
	}
	GPU_SetViewport((u32 *)osConvertVirtToPhys(gpu_depth_fb_addr),
		(u32 *)osConvertVirtToPhys(gpu_fb_addr),
		0, 0, 240, screen_w);

	GPU_DepthMap(-1.0f, 0.0f);
	GPU_SetFaceCulling(GPU_CULL_NONE);
	GPU_SetStencilTest(false, GPU_ALWAYS, 0x00, 0xFF, 0x00);
	GPU_SetStencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
	GPU_SetBlendingColor(0,0,0,0);
	GPU_SetDepthTestAndWriteMask(true, GPU_GEQUAL, GPU_WRITE_ALL);
	GPUCMD_AddMaskedWrite(GPUREG_EARLYDEPTH_TEST1, 0x1, 0);
	GPUCMD_AddWrite(GPUREG_EARLYDEPTH_TEST2, 0);

	GPU_SetAlphaBlending(
		GPU_BLEND_ADD,
		GPU_BLEND_ADD,
		GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA,
		GPU_ONE, GPU_ZERO
	);

	GPU_SetAlphaTest(false, GPU_ALWAYS, 0x00);

	GPU_SetDummyTexEnv(1);
	GPU_SetDummyTexEnv(2);
	GPU_SetDummyTexEnv(3);
	GPU_SetDummyTexEnv(4);
	GPU_SetDummyTexEnv(5);
}

void sf2d_start_frame_target(sf2d_rendertarget *target)
{
	sf2d_pool_reset();
	GPUCMD_SetBufferOffset(0);

	// Upload saved uniform
	matrix_gpu_set_uniform(target->projection, projection_desc);

	int bufferLen = target->texture.width * target->texture.height * 4; // apparently depth buffer is (or can be) 32bit?
	if (bufferLen > targetDepthBufferLen) { // expand depth buffer
		if (targetDepthBufferLen > 0) linearFree(targetDepthBuffer);
		targetDepthBuffer = linearAlloc(bufferLen);
		memset(targetDepthBuffer, 0, bufferLen);
		targetDepthBufferLen = bufferLen;
	}

	GPU_SetViewport((u32 *)osConvertVirtToPhys(targetDepthBuffer),
		(u32 *)osConvertVirtToPhys(target->texture.data),
		0, 0, target->texture.height, target->texture.width);

	currentRenderTarget = target;

	GPU_DepthMap(-1.0f, 0.0f);
	GPU_SetFaceCulling(GPU_CULL_NONE);
	GPU_SetStencilTest(false, GPU_ALWAYS, 0x00, 0xFF, 0x00);
	GPU_SetStencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
	GPU_SetBlendingColor(0,0,0,0);
	GPU_SetDepthTestAndWriteMask(true, GPU_GEQUAL, GPU_WRITE_ALL);
	GPUCMD_AddMaskedWrite(GPUREG_EARLYDEPTH_TEST1, 0x1, 0);
	GPUCMD_AddWrite(GPUREG_EARLYDEPTH_TEST2, 0);

	GPU_SetAlphaBlending(
		GPU_BLEND_ADD,
		GPU_BLEND_ADD,
		GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA,
		GPU_ONE, GPU_ZERO
	);

	GPU_SetAlphaTest(false, GPU_ALWAYS, 0x00);

	GPU_SetDummyTexEnv(1);
	GPU_SetDummyTexEnv(2);
	GPU_SetDummyTexEnv(3);
	GPU_SetDummyTexEnv(4);
	GPU_SetDummyTexEnv(5);
}

void sf2d_end_frame()
{
	GPU_FinishDrawing();
	GPUCMD_Finalize();
	GPUCMD_FlushAndRun();
	gspWaitForP3D();

	if (!currentRenderTarget) {
		//Copy the GPU rendered FB to the screen FB
		if (cur_screen == GFX_TOP) {
			GX_DisplayTransfer(gpu_fb_addr, GX_BUFFER_DIM(240, 400),
				(u32 *)gfxGetFramebuffer(GFX_TOP, cur_side, NULL, NULL),
				GX_BUFFER_DIM(240, 400), 0x1000);
		} else {
			GX_DisplayTransfer(gpu_fb_addr, GX_BUFFER_DIM(240, 320),
				(u32 *)gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL),
				GX_BUFFER_DIM(240, 320), 0x1000);
		}
		gspWaitForPPF();

		//Clear the screen
		GX_MemoryFill(
			gpu_fb_addr, clear_color, &gpu_fb_addr[240*400], GX_FILL_TRIGGER | GX_FILL_32BIT_DEPTH,
			gpu_depth_fb_addr, 0, &gpu_depth_fb_addr[240*400], GX_FILL_TRIGGER | GX_FILL_32BIT_DEPTH);
		gspWaitForPSC0();
	} else {
		//gspWaitForPPF();
		//gspWaitForPSC0();
		sf2d_texture_tile32(&(currentRenderTarget->texture));
	}
	currentRenderTarget = NULL;
}

void sf2d_swapbuffers()
{
	gfxSwapBuffersGpu();
	if (vblank_wait) {
		gspWaitForEvent(GSPGPU_EVENT_VBlank0, false);
	}
	//Calculate FPS
	frames++;
	u64 delta_time = osGetTime() - last_time;
	if (delta_time >= 1000) {
		current_fps = frames/(delta_time/1000.0f);
		frames = 0;
		last_time = osGetTime();
	}
}

void sf2d_set_vblank_wait(int enable)
{
	vblank_wait = enable;
}

float sf2d_get_fps()
{
	return current_fps;
}

void *sf2d_pool_malloc(u32 size)
{
	if ((pool_index + size) < pool_size) {
		void *addr = (void *)((u32)pool_addr + pool_index);
		pool_index += size;
		return addr;
	}
	return NULL;
}

void *sf2d_pool_memalign(u32 size, u32 alignment)
{
	u32 new_index = (pool_index + alignment - 1) & ~(alignment - 1);
	if ((new_index + size) < pool_size) {
		void *addr = (void *)((u32)pool_addr + new_index);
		pool_index = new_index + size;
		return addr;
	}
	return NULL;
}

void *sf2d_pool_calloc(u32 nmemb, u32 size)
{
	return sf2d_pool_memalign(nmemb * size, size);
}

unsigned int sf2d_pool_space_free()
{
	return pool_size - pool_index;
}

void sf2d_pool_reset()
{
	pool_index = 0;
}

void sf2d_set_clear_color(u32 color)
{
	// GX_SetMemoryFill wants the color inverted?
	clear_color =  RGBA8_GET_R(color) << 24 |
		       RGBA8_GET_G(color) << 16 |
		       RGBA8_GET_B(color) <<  8 |
		       RGBA8_GET_A(color) <<  0;
}

void sf2d_set_scissor_test(GPU_SCISSORMODE mode, u32 x, u32 y, u32 w, u32 h)
{
	if (cur_screen == GFX_TOP) {
		GPU_SetScissorTest(mode, 240 - (y + h), 400 - (x + w), 240 - y, 400 - x);
	} else {
		GPU_SetScissorTest(mode, 240 - (y + h), 320 - (x + w), 240 - y, 320 - x);
	}
}

gfxScreen_t sf2d_get_current_screen()
{
	return cur_screen;
}

gfx3dSide_t sf2d_get_current_side()
{
	return cur_side;
}

static void apt_hook_func(APT_HookType hook, void *param)
{
	if (hook == APTHOOK_ONRESTORE) {
		reset_gpu_apt_resume();
	}
}

static void reset_gpu_apt_resume()
{
	GPU_Reset(NULL, gpu_cmd, gpu_cmd_size); // Only required for custom GPU cmd sizes
	shaderProgramUse(&shader);

	if (cur_screen == GFX_TOP) {
		matrix_gpu_set_uniform(ortho_matrix_top, projection_desc);
	} else {
		matrix_gpu_set_uniform(ortho_matrix_bot, projection_desc);
	}

	GPUCMD_Finalize();
	GPUCMD_FlushAndRun();
	gspWaitForP3D();
}
