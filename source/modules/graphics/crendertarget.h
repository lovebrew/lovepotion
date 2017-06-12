#pragma once

namespace love
{
	class CRenderTarget
	{
		public:
			CRenderTarget(int width, int height);
		
			C3D_Mtx projection;
			C3D_RenderTarget * target;
	};
}