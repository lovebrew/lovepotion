local love_math = ...

local floor = math.floor
local min, max = math.min, math.max

--  RANDOM GENERATOR --

local rng = love_math._getRandomGenerator()

function love_math.random(low, high)
    return rng:random(low, high)
end

function love_math.randomNormal(stddev, mean)
    return rng:randomNormal(stddev, mean)
end

function love_math.setRandomSeed(low, high)
    return rng:setSeed(low, high)
end

function love_math.getRandomSeed()
    return rng:getSeed()
end

function love_math.setRandomState(state)
    return rng:setState(state)
end

function love_math.getRandomState()
    return rng:getState()
end

-- COLOR TO / FROM BYTES --

local function clamp01(x)
    return min(max(x, 0), 1)
end

function love_math.colorToBytes(r, g, b, a)
    if type(r) == "table" then
        r, g, b, a = r[1], r[2], r[3], r[4]
    end

    r = floor(clamp01(r) * 255 + 0.5)
    g = floor(clamp01(g) * 255 + 0.5)
    b = floor(clamp01(b) * 255 + 0.5)
    a = (a ~= nil and floor(clamp01(a) * 255 + 0.5)) or nil

    return r, g, b, a
end

function love_math.colorFromBytes(r, g, b, a)
    if type(r) == "table" then
        r, g, b, a = r[1], r[2], r[3], r[4]
    end

    r = clamp01(floor(r + 0.5) / 255)
    g = clamp01(floor(g + 0.5) / 255)
    b = clamp01(floor(b + 0.5) / 255)
    a = (a ~= nil and clamp01(floor(a + 0.5) / 255)) or nil

    return r, g, b, a
end
