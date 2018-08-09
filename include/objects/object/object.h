#pragma once

class Object
{
    public:
        Object() { type = "Object"; };
        Object(const char * name) { type = name; };
        char * ToString();
        void SetType(const char * name) { type = name; };
    
    protected:
        std::string type;
};