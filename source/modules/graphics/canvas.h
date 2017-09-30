#pragma once

#include "crendertarget.h"
#include "image.h"
#include "wrap_graphics.h"

namespace love
{
	class Canvas
	{
		public:
			const char * Init(int width, int height);

			love::CRenderTarget * GetTarget();
			C3D_Mtx * GetProjection();
			love::Image * GetTexture();
			void Clear(u32 color);
			void StartRender();
			void EndRender();

		private:
			int width;
			int height;

			int renderFlag = 0;

			love::CRenderTarget * target;
			love::Image * texture;
	};
}