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

void Canvas::Collect()
{
	C3D_TexDelete(this->texture->GetTexture());

	C3D_RenderTargetDelete(this->target->GetTarget());
}