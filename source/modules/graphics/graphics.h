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
			static int Line(lua_State * L);

			void Render(gfxScreen_t screen);
			void SwapBuffers();

			gfxScreen_t GetScreen();
			CRenderTarget * GetRenderTarget(unsigned int i);
			void InitRenderTargets();

		private:
			Graphics();
			static gfxScreen_t currentScreen;
			static love::CRenderTarget * bottomTarget;
			static int render;
	};
}