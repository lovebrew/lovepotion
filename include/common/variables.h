#pragma once

extern bool ERROR;
extern bool LOVE_QUIT;

extern FILE * logFile;

extern std::vector<std::string> KEYS;

extern std::map<int, std::string> LANGUAGES;

extern std::map<int, std::string> REGIONS;

extern std::vector<std::string> GAMEPAD_AXES;

extern int lastTouch[2];

typedef struct { 
    const char * name; 
    void (* init)(void); 
    int (* reg)(lua_State * L); 
    void (* close)(void); 
} love_modules;

extern love_modules modules[];

extern int (*classes[])(lua_State *L);

#if defined (__SWITCH__)
extern std::vector<HidControllerID> CONTROLLER_IDS;
#endif