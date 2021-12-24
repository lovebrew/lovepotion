R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.
--------------
local function merge(dest, source)
    for _, value in pairs(source) do
        table.insert(dest, value)
    end
end

local function any(t, v)
    for _, value in ipairs(t) do
        if v == value then
            return true
        end
    end
    return false
end

local SerializeTypes = {}
SerializeTypes.TYPE_NUMBER = "n"
SerializeTypes.TYPE_STRING = "z"

local function serialize(t, tablify)
    local pattern = {}
    local values  = {}

    local t_len = #t
    local as_table = tablify ~= nil and true

    for index = 1, t_len do
        local value = t[index]
        local value_type = type(value)

        if value_type == "number" then
            table.insert(pattern, SerializeTypes.TYPE_NUMBER)
            table.insert(values, value)
        elseif any({"string", "nil", "boolean", "userdata"}, value_type) then
            table.insert(pattern, SerializeTypes.TYPE_STRING)
            if value_type == "boolean" then
                table.insert(values, tostring(value))
            elseif value_type == "nil" then
                table.insert(values, "nil")
            elseif value_type =="userdata" then
                table.insert(values, tostring(value))
            else
                table.insert(values, value)
            end
        elseif value_type == "table" then
            if #value ~= 0 then
                local tablePattern, tableValues = serialize(value, true)

                merge(pattern, tablePattern)
                merge(values, tableValues)
            else
                table.insert(pattern, SerializeTypes.TYPE_STRING)
                table.insert(values, tostring(value))
            end
        elseif value_type == "function" then
            local result = { pcall(value) }

            if table.remove(result, 1) then
                local funcPattern, funcValues = serialize(result, true)

                merge(pattern, funcPattern)
                merge(values, funcValues)
            else
                table.insert(pattern, SerializeTypes.TYPE_STRING)
                table.insert(values, tostring(value))
            end
        elseif value_type == "thread" then
            table.insert(pattern, SerializeTypes.TYPE_STRING)
            table.insert(values, tostring(value))
        end
    end

    if not as_table then
        pattern = table.concat(pattern, "x")
    end

    return pattern, values
end
--------------

--[[
- @file console.lua
- @brief Client for nestlink
--]]

local love = require("love")
love.console = {}

-- max amount of retries
local MAX_RETRIES = 3
love.console.retries = 0
love.console.connected = false

-- max time until timeout
local MAX_TIMEOUT = 3

local socket = require("socket")

local _print = print

--[[
- @brief Logs messages with date and time.
- @param `format` -> Message format, see https://www.lua.org/pil/20.2.html.
- @param `...` -> Variadic args for `format`.
--]]
local function log(format, ...)
    local dateTime = os.date("%Y-%m-%d/%H:%M:%S")

    local logFormat = "[%s] %s"
    local logString = string.format(logFormat, dateTime, string.format(format, ...))
    _print(logString)

    if love.console.logfile then
        love.console.logfile:write(logString .. "\n")
    end
end

--[[
- @brief `print` overriding function.
- @param args: `...` -> variadic args to print.
--]]
function print(...)
    local arg = {...}

    local pattern, values = serialize(arg)
    local data = love.data.pack("string", pattern, unpack(values))

    love.console:send(pattern .. "/" .. data)
end

--[[
- @brief Attempt to connect to `host`:`port` server.
- @param `host` -> The IP address from the `love.conf` console field.
- @param `port` -> The port number from the `love.conf` console field.
- @return `success` -> boolean.
--]]
function love.console:tryConnection(host, port)
    local success = self.socket:connect(host, port)

    return success
end

--[[
- @brief Initialize the client protocol.
- @param `host` -> The IP address from the `love.conf` console field.
- @param `_port` -> The port number from the `love.conf` console field.
- @param `_debugMode` -> Whether to enable file logging for the console.
- @note File logging can slow down your game!
--]]
function love.console:init(host, _port, _debugMode)
    self.socket = socket.tcp()
    assert(self.socket, "failed to create socket")

    if love.filesystem and _debugMode then
        self.logfile = love.filesystem.newFile("nestlink.log", "w")
    end

    self.socket:settimeout(MAX_TIMEOUT)

    local port = _port or 8000
    while self.retries < MAX_RETRIES do
        local success = self:tryConnection(host, port)

        if not success then
            self.retries = self.retries + 1
            log("Connection to Remote timed out, retrying: %d/%d", self.retries, MAX_RETRIES)
        else
            love.console.connected = true
            break
        end
    end

    if self.retries == MAX_RETRIES and not love.console.connected then
        return log("Failed to connect to Remote %s:%d", host, port)
    end
    log("Successfully connected to Remote %s:%d", host, port)
end

--[[
- @brief Send a string of data to the server.
- @param `data` -> Datagram message to send.
--]]
function love.console:send(data)
    if not self.connected then
        return
    end

    local success, message = self.socket:send(data .. "\n")

    if not success then
        return log("Failed to send '%s' to server: %s", data, message)
    end
    log("Sending '%s' to server", data)
end

function love.console:close()
    if not self.socket then
        return
    end

    self.socket:shutdown()
    self.socket:close()
end

-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
