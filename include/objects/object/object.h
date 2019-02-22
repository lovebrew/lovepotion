#pragma once

class Object
{
    public:
        Object() { type = "Object"; };
        Object(const std::string & name) { type = name; };
        char * ToString();
        void SetType(const std::string & name) { type = name; };
    
    protected:
        std::string type;
};