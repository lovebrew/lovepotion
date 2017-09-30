#include "common/runtime.h"
#include "canvas.h"

using love::Canvas;

const char * Canvas::Init(int width, int height)
{
	this->width = width;
	this->height = height;

	this->texture = new love::Image();
	this->texture->BasicInit(width, height);

	this->target = new love::CRenderTarget(this->texture, this->texture->NextPow2(width), this->texture->NextPow2(height));

	return nullptr;
}

void Canvas::StartRender()
{
	if (!this->renderFlag)
	{
		C3D_FrameBegin(0);

		C3D_FrameDrawOn(this->target->GetTarget());
		
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, projection_desc, this->GetProjection());
		
		//this->Clear(graphicsGetBackgroundColor());

		this->renderFlag = 1;
	}
}

void Canvas::EndRender()
{
	if (this->renderFlag)
	{
		C3D_FrameEnd(0);
		
		this->renderFlag = 0;
	}
}

void Canvas::Clear(u32 color)
{
	this->target->Clear(color);
}

love::CRenderTarget * Canvas::GetTarget()
{
	return this->target;
}

C3D_Mtx * Canvas::GetProjection()
{
	return this->target->GetProjection();
}

love::Image * Canvas::GetTexture()
{
	return this->texture;
}