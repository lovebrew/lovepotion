R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.

-- Based on the code from https://github.com/rxi/log.lua
-- LICENSE: https://github.com/rxi/log.lua/blob/master/LICENSE

-- make sure love exists.
local love  = require("love")
require("love.arg")

local getinfo = debug.getinfo

-- to get the working directory
require("love.filesystem")

local log = {}
log.__index = log

log.format = "[%-6s%s] %s:\n%s\n"
log.level = "trace"

local modes =
{
    trace = 0,
    debug = 1,
    info  = 2,
    warn  = 3,
    error = 4,
    fatal = 5
}

function log.new(filename)
    local instance = setmetatable({}, log)

    local path = love.path.getFull(filename)
    instance.file = io.open(path, "w")

    return instance
end

for level, _ in pairs(modes) do
    log[level] = function(self, ...)
        if not self.file then
            return
        end

        if modes[self.level] < modes[level] then
            return
        end

        local message = tostring(...)
        local info = getinfo(2, "Sl")

        local line = ("%s:%d"):format(info.short_src, info.currentline)
        local buffer = log.format:format(level:upper(), os.date(), line, message)

        self.file:write(buffer)
        self.file:flush()
    end
end

return setmetatable(log, {
    __call = function(_, ...)
        return log.new(...)
    end
})

-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
