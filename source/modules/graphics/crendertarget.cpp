#include "common/runtime.h"
#include "crendertarget.h"

using love::CRenderTarget;

CRenderTarget::CRenderTarget(int width, int height)
{
	this->target = C3D_RenderTargetCreate(height, width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	Mtx_Ortho(&this->projection, 0.0f, width, 0.0f, height, 0.0f, 1.0f, true);
	C3D_RenderTargetSetClear(this->target, C3D_CLEAR_ALL, 0x68B0D8FF, 0);
}