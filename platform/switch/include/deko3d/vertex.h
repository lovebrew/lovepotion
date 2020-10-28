#pragma once

#include "deko3d/CMemPool.h"
#include "deko3d/common.h"
#include "common/colors.h"

#include <array>
#include <memory>

namespace vertex
{
    struct Vertex
    {
        float position[3];
        float color[4];
    };
};