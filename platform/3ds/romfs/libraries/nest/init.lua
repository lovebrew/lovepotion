local PATH = (...):gsub('%.init$', '')

local nest =
{
    _VERSION     = "0.1.0",
    _DESCRIPTION = "Löve Potion Compatabiility Layer library",
    _LICENSE     =
    [[
        MIT LICENSE

        Copyright (c) 2020 Jeremy S. Postelnek / TurtleP

        Permission is hereby granted, free of charge, to any person obtaining a
        copy of this software and associated documentation files (the
        "Software"), to deal in the Software without restriction, including
        without limitation the rights to use, copy, modify, merge, publish,
        distribute, sublicense, and/or sell copies of the Software, and to
        permit persons to whom the Software is furnished to do so, subject to
        the following conditions:

        The above copyright notice and this permission notice shall be included
        in all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
        OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
        MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
        IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
        CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
        TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
        SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    ]]
}

--- Load the device to use.
--- @param device {string}: ctr for 3DS, nx for Switch
nest.init = function(device, options)
    -- make sure we're not already
    -- running under homebrew!
    if love._console_name then
        return
    end

    assert(device ~= nil, "Bad argument #1 to init: string expected, got " .. tostring(device))
    assert(device:match("ctr") or device:match("nx"), "Invalid device " .. device .. " expected 'ctr' or 'nx'")

    if not options.no_override then
        -- override the LÖVE variables
        -- don't actually do this other people
        -- this is dangerous and not recommended!
        love._os = "Horizon"
        love._console_name = (device == "ctr" and "3DS") or "Switch"
    end

    love._potion_version = "2.0.0"

    -- format a path to the appropriate device folder
    local module_folder = string.format("%s.%s", PATH, device)
    local dimensions = require(module_folder)

    love.window.updateMode(unpack(dimensions))
end

return nest
