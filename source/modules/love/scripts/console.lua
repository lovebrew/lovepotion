R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.
--------------
--
-- json.lua
--
-- Copyright (c) 2020 rxi
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy of
-- this software and associated documentation files (the "Software"), to deal in
-- the Software without restriction, including without limitation the rights to
-- use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
-- of the Software, and to permit persons to whom the Software is furnished to do
-- so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.
--

local json = { _version = "0.1.2" }

local encode

local escape_char_map = {
  [ "\\" ] = "\\",
  [ "\"" ] = "\"",
  [ "\b" ] = "b",
  [ "\f" ] = "f",
  [ "\n" ] = "n",
  [ "\r" ] = "r",
  [ "\t" ] = "t",
}

local escape_char_map_inv = { [ "/" ] = "/" }
for k, v in pairs(escape_char_map) do
  escape_char_map_inv[v] = k
end

local function escape_char(c)
  return "\\" .. (escape_char_map[c] or string.format("u%04x", c:byte()))
end

local function encode_nil(val)
  return "null"
end


local function encode_table(val, stack)
  local res = {}
  stack = stack or {}

  -- Circular reference?
  if stack[val] then error("circular reference") end

  stack[val] = true

  if rawget(val, 1) ~= nil or next(val) == nil then
    -- Treat as array -- check keys are valid and it is not sparse
    local n = 0
    for k in pairs(val) do
      if type(k) ~= "number" then
        error("invalid table: mixed or invalid key types")
      end
      n = n + 1
    end
    if n ~= #val then
      error("invalid table: sparse array")
    end
    -- Encode
    for i, v in ipairs(val) do
      table.insert(res, encode(v, stack))
    end
    stack[val] = nil
    return "[" .. table.concat(res, ",") .. "]"
  else
    -- Treat as an object
    for k, v in pairs(val) do
      if type(k) ~= "string" then
        error("invalid table: mixed or invalid key types")
      end
      table.insert(res, encode(k, stack) .. ":" .. encode(v, stack))
    end
    stack[val] = nil
    return "{" .. table.concat(res, ",") .. "}"
  end
end

local function encode_string(val)
  return '"' .. val:gsub('[%z\1-\31\\"]', escape_char) .. '"'
end

local function encode_number(val)
  -- Check for NaN, -inf and inf
  if val ~= val or val <= -math.huge or val >= math.huge then
    error("unexpected number value '" .. tostring(val) .. "'")
  end
  return string.format("%.14g", val)
end

local type_func_map = {
  [ "nil"     ] = encode_nil,
  [ "table"   ] = encode_table,
  [ "string"  ] = encode_string,
  [ "number"  ] = encode_number,
  [ "boolean" ] = tostring,
}

encode = function(val, stack)
  local t = type(val)
  local f = type_func_map[t]
  if f then
    return f(val, stack)
  end
  error("unexpected type '" .. t .. "'")
end

function json.encode(val)
  return ( encode(val) )
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
    local arg, results = ..., nil
    local length = select("#", ...)

    if type(arg) == "function" then
        results = { pcall(arg) }
        if not table.remove(results, 1) then
            log("Function call failed.")
        end
    elseif length > 1 then
        results = table.concat({...}, ", ")
        return love.console:send(results)
    elseif type(arg) == "nil" then
        results = json.encode(nil)
        return love.console:send(results)
    elseif type(arg) == "table" then
        results = arg
    else
        results = { ... }
    end

    love.console:send(json.encode(results))
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
- @param `port` -> The port number from the `love.conf` console field.
--]]
function love.console:init(host, _port)
    self.socket = socket.tcp()
    assert(self.socket, "failed to create socket")

    if love.filesystem then
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

-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
