local ImageData_mt = ...
local ImageData = ImageData_mt.__index

local tonumber, assert, error = tonumber, assert, error
local type, pcall = type, pcall
local floor = math.floor
local min, max = math.min, math.max

local function inside(x, y, w, h)
    return x >= 0 and x < w and y >= 0 and y < h
end

local function clamp01(x)
    return min(max(x, 0), 1)
end

-- Implement thread-safe ImageData:mapPixel regardless of whether the FFI is
-- used or not.
function ImageData:mapPixel(func, ix, iy, iw, ih)
    local idw, idh = self:getDimensions()

    ix = ix or 0
    iy = iy or 0
    iw = iw or idw
    ih = ih or idh

    if type(ix) ~= "number" then error("bad argument #2 to ImageData:mapPixel (expected number)", 2) end
    if type(iy) ~= "number" then error("bad argument #3 to ImageData:mapPixel (expected number)", 2) end
    if type(iw) ~= "number" then error("bad argument #4 to ImageData:mapPixel (expected number)", 2) end
    if type(ih) ~= "number" then error("bad argument #5 to ImageData:mapPixel (expected number)", 2) end

    if type(func) ~= "function" then error("bad argument #1 to ImageData:mapPixel (expected function)", 2) end
    if not (inside(ix, iy, idw, idh) and inside(ix + iw - 1, iy + ih - 1, idw, idh)) then error("Invalid rectangle dimensions", 2) end

    -- performAtomic and mapPixelUnsafe have Lua-C API and FFI versions.
    self:_performAtomic(self._mapPixelUnsafe, self, func, ix, iy, iw, ih)
end
