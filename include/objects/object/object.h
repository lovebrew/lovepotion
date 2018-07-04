#pragma once

class Object
{
    public:
        Object() { type = "Object"; };
        Object(char * name) { type = name; };
        char * ToString();
    
    protected:
        char * type;
};