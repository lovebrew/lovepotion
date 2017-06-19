#pragma once

namespace love
{
	class Timer
	{
		static love::Timer * instance;

		public:
			static love::Timer * Instance()
			{
				if (!instance)
					instance = new love::Timer();
				return instance;
			}

			static int GetTime(lua_State * L);
			static int GetDelta(lua_State * L);
			static int GetFPS(lua_State * L);
			static int Step(lua_State * L);
			void Tick();

		private:
			Timer();

			static float dt;
			static int lastTick;
			static int currentTick;

			static float fps;
			u64 lastCountTime;
			int frames;
	};
}