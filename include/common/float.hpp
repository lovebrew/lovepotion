/**
 * Copyright (c) 2006-2024 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#pragma once

#include "common/int.hpp"

namespace love
{

    typedef uint16_t float16_t;
    typedef uint16_t float11_t;
    typedef uint16_t float10_t;

    void float16Init();

    float float16to32(float16_t f);
    float16_t float32to16(float f);

    float float11to32(float11_t f);
    float11_t float32to11(float f);

    float float10to32(float10_t f);
    float10_t float32to10(float f);

} // namespace love
