#pragma once

namespace love
{
	class Filesystem
	{
		static love::Filesystem * instance;

		public:
			static int SetIdentity(lua_State * L);
			static int GetIdentity(lua_State * L);
			static int GetSize(lua_State * L);
			static int GetSaveDirectory(lua_State * L);
			static int IsFile(lua_State * L);
			static int IsDirectory(lua_State * L);
			static int GetDirectoryItems(lua_State * L);
			static int Read(lua_State * L);
			static int Write(lua_State * L);
			static int Load(lua_State * L);
	
			char * GetSaveDirectory();
			const char * GetIdentity();
			const char * Redirect(const char * path);

			static love::Filesystem * Instance() {
				if (!instance)
					instance = new love::Filesystem;
				return instance;
			}

		private:
			static const char * identity;
			Filesystem();
	};
}