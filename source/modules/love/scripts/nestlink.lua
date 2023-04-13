R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.
--------------
local socket = require("socket")

---@class client
local client = {}
client.socket = nil
client.separator = "␟"
client.retries = 0
client.max_retries = 3

local function visit(raw_value, visited, result, path, inspect)
    if visited[raw_value] then
        table.insert(result, path .. " = {...}")
    else
        visited[raw_value] = true
        if #path > 0 then
            path = path .. "."
        end
        table.insert(result, path .. "{")
        for key, value in pairs(raw_value) do
            local key_string, value_string = inspect(key, visited, path .. " "), inspect(value, visited, path .. " ")
            table.insert(result, ("%s [%s] = %s"):format(path, key_string, value_string))
        end
        table.insert(result, path .. "}")
    end
end

local function inspect(value, visited, path)
    local value_type = type(value)

    if value_type == "nil" then
        return "nil"
    elseif value_type == "boolean" then
        return value and "true" or "false"
    elseif value_type == "number" then
        return tostring(value)
    elseif value_type == "string" then
        return value:format("%q")
    elseif value_type == "function" then
        local info = debug.getinfo(value, "S")
        local source = info.source or "=[C]"
        return ("function %s:%d"):format(source, info.linedefined)
    elseif value_type == "table" then
        local result = {}
        visit(value, visited, result, path, inspect)
        return table.concat(result, "\n")
    elseif value_type == "thread" then
        return ("thread: %s"):format(tostring(value))
    else
        return tostring(value)
    end
end

function client.connect(host, port)
    local tcp_socket, _ = socket.tcp()
    if not tcp_socket then
        return
    end

    client.socket = tcp_socket
    client.socket:settimeout(5)

    local success, _ = client.socket:connect(host, port)
    if not success then
        return
    end

    client.socket:settimeout(0)

    function print(...)
        local args = { ... }
        local stringified = {}

        for index = 1, #args do
            stringified[index] = inspect(args[index])
        end
        client.socket:send(table.concat(stringified, client.separator) .. "\n")
    end
end

function client.disconnect()
    if not client.socket then
        return
    end

    client.socket:shutdown()
    client.socket:close()
end

return client
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
