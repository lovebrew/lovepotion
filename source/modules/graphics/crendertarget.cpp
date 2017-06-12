#include "common/runtime.h"
#include "crendertarget.h"

using love::CRenderTarget;

CRenderTarget::CRenderTarget(int width, int height)
{
	this->target = C3D_RenderTargetCreate(height, width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	Mtx_OrthoTilt(&this->projection, 0.0f, width, height, 0.0f, 0.0f, 1.0f, true);
}