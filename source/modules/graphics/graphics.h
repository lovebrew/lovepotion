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
			static int SetFont(lua_State * L);

			static int Line(lua_State * L);
			static int Rectangle(lua_State * L);
			static int Circle(lua_State * L);
			static int Points(lua_State * L);
			static int Draw(lua_State * L);
			static int Print(lua_State * L);
			static int SetColor(lua_State * L);
			static int SetBackgroundColor(lua_State * L);
			static int GetBackgroundColor(lua_State * L);

			void Render(gfxScreen_t screen);
			void SwapBuffers();
			void StartTarget(love::CRenderTarget * target);

			gfxScreen_t GetScreen();
			void InitRenderTargets();

		private:
			Graphics();
			static gfxScreen_t currentScreen;
			static gfxScreen_t renderScreen;

			static u32 backgroundColor;

			love::CRenderTarget * bottomTarget = nullptr;
			love::CRenderTarget * topTarget = nullptr;

			bool inRender = false;
	};
}