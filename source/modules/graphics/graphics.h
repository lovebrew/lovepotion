#pragma once

namespace love
{
	class Graphics
	{
		static love::Graphics * instance;

		public:
			~Graphics();
			static love::Graphics * Instance()
			{
				if (!instance)
					instance = new love::Graphics();
				return instance;
			}

			static int GetWidth(lua_State * L);
			static int GetHeight(lua_State * L);
			static int SetScreen(lua_State * L);
			static int Present(lua_State * L);

			gfxScreen_t GetScreen();
			C3D_RenderTarget * GetRenderTarget(unsigned int i);
			void InitRenderTargets();

		private:
			Graphics();
			static gfxScreen_t currentScreen;
			static love::CRenderTarget * bottomTarget;

	};
}