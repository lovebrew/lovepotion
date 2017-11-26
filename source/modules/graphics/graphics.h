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
			static int GetFont(lua_State * L);
			static int Set3D(lua_State * L);
			static int SetDepth(lua_State * L);
	
			static int Line(lua_State * L);
			static int Rectangle(lua_State * L);
			static int Circle(lua_State * L);
			static int Points(lua_State * L);
			static int Draw(lua_State * L);
			static int Print(lua_State * L);
			static int Printf(lua_State * L);
			static int Push(lua_State * L);
			static int Pop(lua_State * L);
			static int Translate(lua_State * L);
			static int SetScissor(lua_State * L);
			static int SetDefaultFilter(lua_State * L);
			static int SetCanvas(lua_State * L);
			static int GetRendererInfo(lua_State * L);

			static int SetColor(lua_State * L);
			static int SetBackgroundColor(lua_State * L);
			static int GetBackgroundColor(lua_State * L);
			static int Clear(lua_State * L);
			
			void Render(gfxScreen_t screen, gfx3dSide_t side);
			void SwapBuffers();
			void StartTarget(love::CRenderTarget * target);
			void Clear(gfxScreen_t screen, gfx3dSide_t side);

			void EnsureInRender();

			gfxScreen_t GetScreen();
			gfx3dSide_t GetSide();
			void InitRenderTargets();
			bool InFrame();

		private:
			Graphics();

			static u32 backgroundColor;

			love::CRenderTarget * bottomTarget = nullptr;
			love::CRenderTarget * topTarget = nullptr;
			love::CRenderTarget * topDepthTarget = nullptr;
			bool inRender = false;
			
	};
}