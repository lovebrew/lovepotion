#pragma once

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    #include <compat-5.3.h>
    #include <lutf8lib.h>
}

// custom things

struct Nil {};

enum DoneAction
{
    DONE_QUIT,
    DONE_RESTART
};

extern bool g_accelJoystick;

// std stuff
#include <algorithm>
#include <array>
#include <math.h>
#include <queue>
#include <map>
#include <atomic>
#include <bitset>
#include <sys/time.h>
#include <filesystem>
#include <limits>
#include <malloc.h>
#include <unistd.h>
#include <list>
#include <set>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#include <string>

#include <utility>
#include <vector>
#include <variant>

#include "common/defines.h"
#include "common/strongref.h"
#include "common/type.h"
#include "common/proxy.h"
#include "common/module.h"
#include "common/colors.h"

#define DEBUG 1
#include "common/logger.h"
#include "common/luax.h"

#include "common/data.h"
#include "common/variant.h"
#include "common/message.h"
