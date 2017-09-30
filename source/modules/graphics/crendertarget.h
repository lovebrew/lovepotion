#pragma once

#include "image.h"

namespace love
{
	class CRenderTarget
	{
		public:
			CRenderTarget(gfxScreen_t screen, gfx3dSide_t side, int width, int height);
			CRenderTarget(love::Image * texture, int width, int height);

			void Clear(u32 color);

			C3D_Mtx * GetProjection();
			C3D_RenderTarget * GetTarget();
			
		private:
			C3D_Mtx projection;
			C3D_RenderTarget * target;
	};
}