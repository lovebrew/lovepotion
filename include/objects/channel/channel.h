#pragma once 

class Channel : public Object
{
	public:
		Channel() {};
		Channel(const std::string & name);

		void Push(Variant variant);
		void Pop(lua_State * L);

	private:
		std::queue<Variant> content;
		std::string name;
};

extern std::map<std::string, Channel *> channels;