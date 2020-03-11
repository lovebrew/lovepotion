#pragma once

extern "C"
{
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
#include <stack>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#include <string>
using namespace std::string_literals;

#include <utility>
#include <vector>
#include <variant>

#include "common/strongref.h"
#include "common/type.h"
#include "common/defines.h"
#include "common/proxy.h"
#include "common/module.h"

#include "common/logger.h"
#include "common/luax.h"

#include "common/data.h"
#include "common/variant.h"
#include "common/message.h"
