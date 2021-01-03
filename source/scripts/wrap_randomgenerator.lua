local rng_mt = ...

local RandomGenerator = rng_mt.__index
local _random = RandomGenerator._random

local type, tonumber, error = type, tonumber, error
local floor = math.floor

local function getrandom(r, l, u)
    if u ~= nil then
        if type(r) ~= "number" then
            error("bad argument #1 to 'random' (number expected)", 2)
        end

        if type(l) ~= "number" then
            error("bad argument #2 to 'random' (number expected)", 2)
        end

        return floor(r * (u - l + 1)) + l
    elseif l ~= nil then
        if type(l) ~= "number" then
            error("bad argument #1 to 'random' (number expected)", 2)
        end

        return floor(r * l) + 1
    else
        return r
    end
end

function RandomGenerator:random(l, u)
    local r = _random(self)
    return getrandom(r, l, u)
end
