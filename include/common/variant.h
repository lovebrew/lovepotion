#pragma once

class Variant
{
    public:
        Variant();
        Variant(void * userdata);
        Variant(double number);
        Variant(const std::string & sstring);
        Variant(bool boolean);

        ~Variant();

        static Variant FromLua(lua_State * L, int index, int type);
        void ToLua(lua_State * L);

    private:
        VARIANTS tag;
        
        union
        {
            double data_number;
            char * data_string;
            bool   data_boolean;
            void * data_pointer;
        };
};