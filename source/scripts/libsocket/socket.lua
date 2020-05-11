local socket = require("socket.core")
local _M = socket

local INADDR_ANY = "0.0.0.0"
function _M.bind(host, port, backlog)
    if host == "*" then
        host = INADDR_ANY
    end

    local sock, result = socket.tcp()

    if not sock then
        return nil, result
    end

    sock:setoption("reuseaddr", true)
    result, error = sock:bind(host, port)

    if not result then
        sock:close()
    else
        result, error = sock:listen(backlog)

        if not result then
            sock:close()
        else
            return sock
        end
    end

    return nil, error
end

return _M
