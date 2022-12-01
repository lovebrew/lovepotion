R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.

--[[
Copyright (c) 2006-2021 LOVE Development Team

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
--]]

local love          = require("love")
local is_debug, log = pcall(require, "love.log")
local file          = nil
if is_debug then
    file = log.new("callbacks.log")
end

function love.createhandlers()

    -- Standard callback handlers.
    love.handlers = setmetatable({
        -- REGION UNUSED --
        keypressed = function(button, scancode, isRepeat)
            if love.keypressed then
                return love.keypressed(button, scancode, isRepeat)
            end
        end,
        keyreleased = function(button, scancode)
            if love.keyreleased then
                return love.keyreleased(button, scancode)
            end
        end,
        -- END REGION UNUSED --
        textinput = function(text)
            if love.textinput then
                return love.textinput(text)
            end
        end,
        -- REGION UNUSED --
        textedited = function(text, start, length)
            if love.textedited then
                return love.textedited(text, start, length)
            end
        end,
        mousemoved = function(x, y, dx, dy, isTouch)
            if love.mousemoved then
                return love.mousemoved(x, y, dx, dy, isTouch)
            end
        end,
        mousepressed = function(x, y, button, isTouch, presses)
            if love.mousepressed then
                return love.mousepressed(x, y, button, isTouch, presses)
            end
        end,
        mousereleased = function(x, y, button, isTouch, presses)
            if love.mousereleased then
                return love.mousereleased(x, y, button, isTouch, presses)
            end
        end,
        wheelmoved = function(x, y)
            if love.wheelmoved then
                return love.wheelmoved(x, y)
            end
        end,
        -- END REGION UNUSED
        touchpressed = function(id, x, y, dx, dy, pressure)
            if love.touchpressed then
                return love.touchpressed(id, x, y, dx, dy, pressure)
            end
        end,
        touchreleased = function(id, x, y, dx, dy, pressure)
            if love.touchreleased then
                return love.touchreleased(id, x, y, dx, dy, pressure)
            end
        end,
        touchmoved = function(id, x, y, dx, dy, pressure)
            if love.touchmoved then
                return love.touchmoved(id, x, y, dx, dy, pressure)
            end
        end,
        joystickpressed = function(joystick, button)
            if love.joystickpressed then
                return love.joystickpressed(joystick, button)
            end
        end,
        joystickreleased = function(joystick, button)
            if love.joystickreleased then
                return love.joystickreleased(joystick, button)
            end
        end,
        joystickaxis = function(joystick, axis, value)
            if love.joystickaxis then
                return love.joystickaxis(joystick, axis, value)
            end
        end,
        -- REGION UNIMPLEMENTED --
        joystickhat = function(joystick, hat, value)
            if love.joystickhat then
                return love.joystickhat(joystick, hat, value)
            end
        end,
        -- END REGION UNIMPLEMENTED --
        gamepadpressed = function(joystick, button)
            if love.gamepadpressed then
                return love.gamepadpressed(joystick, button)
            end
        end,
        gamepadreleased = function(joystick, button)
            if love.gamepadreleased then
                return love.gamepadreleased(joystick, button)
            end
        end,
        gamepadaxis = function(joystick, axis, value)
            if love.gamepadaxis then
                return love.gamepadaxis(joystick, axis, value)
            end
        end,
        joystickadded = function(joystick)
            if love.joystickadded then
                return love.joystickadded(joystick)
            end
        end,
        joystickremoved = function(joystick)
            if love.joystickremoved then
                return love.joystickremoved(joystick)
            end
        end,
        focus = function(focused)
            if love.focus then
                return love.focus(focused)
            end
        end,
        -- REGION UNUSED --
        mousefocus = function(mouseFocused)
            if love.mousefocus then
                return love.mousefocus(mouseFocused)
            end
        end,
        -- END REGION UNUSED --
        visible = function(visible)
            if love.visible then
                return love.visible(visible)
            end
        end,
        quit = function()
            return
        end,
        threaderror = function(thread, error)
            if love.threaderror then
                return love.threaderror(thread, error)
            end
        end,
        resize = function(width, height)
            if love.resize then
                return love.resize(width, height)
            end
        end,
        -- REGION UNUSED --
        filedropped = function(file)
            if love.filedropped then
                return love.filedropped(file)
            end
        end,
        directorydropped = function(directory)
            if love.directorydropped then
                return love.directorydropped(directory)
            end
        end,
        -- END REGION UNUSED --
        lowmemory = function()
            if love.lowmemory then
                love.lowmemory()
            end
            collectgarbage()
            collectgarbage()
        end,
        displayrotated = function(display, orient)
            if love.displayrotated then
                return love.displayrotated(display, orient)
            end
        end,
    }, {
        __index = function(self, name)
            error("Unknown event: " .. name)
        end,
    })

end

-----------------------------------------------------------
-- Default callbacks.
-----------------------------------------------------------

function love.run()
    if love.load then
        love.load(arg)
    end

    if love.timer then
        love.timer.step()
    end

    local delta = 0

    return function()
        if love.window and g_windowShown then
            return
        end

        if love.event and love.event.pump then
            love.event.pump()

            for name, a, b, c, d, e, f in love.event.poll() do
                if name == "quit" then
                    if not love.quit or not love.quit() then
                        love.audio.stop()
                        return a or 0
                    end
                end
                love.handlers[name](a, b, c, d, e, f)
            end
        end

        if love.timer then
            delta = love.timer.step()
        end

        if love.update then
            love.update(delta)
        end

        if love.graphics and love.graphics.isActive() then
            local screens = love.graphics.getScreens()

            for _, screen in ipairs(screens) do
                love.graphics.origin()

                love.graphics.setActiveScreen(screen)
                love.graphics.clear(love.graphics.getBackgroundColor())

                if love.draw then
                    love.draw(screen)
                end
            end

            love.graphics.present()
        end

        if love.timer then
            love.timer.sleep(0.001)
        end
    end
end

local debug, print, tostring, error = debug, print, tostring, error

function love.threaderror(t, err)
    error("Thread error (" .. tostring(t) .. ")\n\n" .. err, 0)
end

local utf8 = require("utf8")

local function error_printer(msg, layer)
    local trace = debug.traceback("Error: " .. tostring(msg), 1 + (layer or 1)):gsub("\n[^\n]+$", "")
    print(trace)

    if file then
        file:echo(trace)
    end
end

function love.errorhandler(msg)
    msg = tostring(msg)

    error_printer(msg, 2)

    if not love.window or not love.graphics or not love.event then
        return
    end

    if not love.graphics.isCreated() or not love.window.isOpen() then
        local success, status = pcall(love.window.setMode, 800, 600)
        if not success or not status then
            return
        end
    end

    -- Reset state.
    if love.mouse then
        love.mouse.setVisible(true)
        love.mouse.setGrabbed(false)
        love.mouse.setRelativeMode(false)
        if love.mouse.isCursorSupported() then
            love.mouse.setCursor()
        end
    end

    if love.joystick then
        -- Stop all joystick vibrations.
        for i, v in ipairs(love.joystick.getJoysticks()) do
            v:setVibration()
        end
    end

    if love.audio then
        love.audio.stop()
    end

    love.graphics.reset()
    local font = love.graphics.newFont(14)

    love.graphics.setColor(1, 1, 1)

    local trace = debug.traceback()

    love.graphics.origin()

    local sanitizedmsg = {}
    for char in msg:gmatch(utf8.charpattern) do
        table.insert(sanitizedmsg, char)
    end
    sanitizedmsg = table.concat(sanitizedmsg)

    local err = {}

    table.insert(err, "Error\n")
    table.insert(err, sanitizedmsg)

    if #sanitizedmsg ~= #msg then
        table.insert(err, "Invalid UTF-8 string in error message.")
    end

    table.insert(err, "\n")

    for l in trace:gmatch("(.-)\n") do
        if not l:match("boot.lua") then
            l = l:gsub("stack traceback:", "Traceback\n")
            table.insert(err, l)
        end
    end

    local p = table.concat(err, "\n")

    p = p:gsub("\t", "")
    p = p:gsub("%[string \"(.-)\"%]", "%1")

    local screens = love.graphics.getScreens()

    local _, text = font:getWrap(p, love.graphics.getWidth() - 20)

    if #text > 14 then
        for index = 15, #text do
            text[index] = nil
        end
    end

    table.insert(text, "")

    local not_saved_message = "Press Start to quit or A save this error."
    local saved_message = "Error saved. Press Start to quit."

    table.insert(text, not_saved_message)

    local function draw()
        if not love.graphics.isActive() then
            return
        end

        for _, screen in ipairs(screens) do
            love.graphics.origin()

            love.graphics.setActiveScreen(screen)
            love.graphics.clear(0.35, 0.62, 0.86)

            if screen ~= "bottom" then
                for index = 1, #text do
                    love.graphics.print(text[index], font, 5, (index - 1) * 16)
                end
            end
        end

        love.graphics.present()
    end

    local fullErrorText, savedMessage = p, false
    local function saveErrorToFile()
        if savedMessage then
            return
        end

        local date = os.date("%H%M%S_%m%d%y")
        local filename = string.format("errors/love_error_%s.txt", date)

        if not love.filesystem then
            return
        end

        love.filesystem.createDirectory("errors")
        love.filesystem.write(filename, fullErrorText)

        text[#text] = saved_message
        savedMessage = true
    end

    return function()
        love.event.pump()

        for e, a, b, c in love.event.poll() do
            if e == "quit" then
                return 1
            elseif e == "gamepadpressed" and b == "start" then
                return 1
            elseif e == "gamepadpressed" and b == "a" then
                saveErrorToFile()
            elseif e == "touchpressed" then
                local name = love.filesystem.getIdentity()

                if #name == 0 or name == "Untitled" then
                    name = "Game"
                end

                local buttons = { "OK", "Cancel" }

                local pressed = love.window.showMessageBox("Quit " .. name .. "?", "", buttons)

                if pressed == 1 then
                    return 1
                end
            end
        end

        draw()

        if love.timer then
            love.timer.sleep(0.1)
        end
    end
end

love.errhand = love.errorhandler

-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
