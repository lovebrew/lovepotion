class Exception : public std::exception
{
	public:
		Exception(const std::string & format, ...);
		Exception(const char * message);
		Exception(lua_State * L);

		virtual ~Exception() throw();

		inline virtual const char * what() const throw()
		{
			return message.c_str();
		}
	
	private:
		std::string message;
};