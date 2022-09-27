R"luastring"--(
-- make sure love exists.
local love  = require("love")
require("love.arg")

local debug = debug.getinfo

-- to get the working directory
require("love.filesystem")

local log = {}
log.__index = log

log.format = "%s:%d:\n%s\n\n"

function log.new(filename)
    local instance = setmetatable({}, log)

    local path = love.path.getFull(filename)
    instance.file = io.open(path, "w")

    return instance
end

function log:echo(format, ...)
    if not self.file then
        return
    end

    local caller_name = debug(2).short_src
    local line_number = debug(2).currentline

    local buffer = string.format(format, ...)
    self.file:write(string.format(log.format, caller_name, line_number, buffer))
end

return log
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
