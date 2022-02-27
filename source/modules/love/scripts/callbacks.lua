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

local love = require("love")

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
        textedited = function (text, start, length)
            if love.textedited then
                return love.textedited(text, start, length)
            end
        end,
        mousemoved = function (x, y, dx, dy, isTouch)
            if love.mousemoved then
                return love.mousemoved(x, y, dx, dy, isTouch)
            end
        end,
        mousepressed = function (x, y, button, isTouch, presses)
            if love.mousepressed then
                return love.mousepressed(x, y, button, isTouch, presses)
            end
        end,
        mousereleased = function (x, y, button, isTouch, presses)
            if love.mousereleased then
                return love.mousereleased(x, y, button, isTouch, presses)
            end
        end,
        wheelmoved = function (x, y)
            if love.wheelmoved then
                return love.wheelmoved(x, y)
            end
        end,
        -- END REGION UNUSED
        touchpressed = function (id, x, y, dx, dy, pressure)
            if love.touchpressed then
                return love.touchpressed(id, x, y, dx, dy, pressure)
            end
        end,
        touchreleased = function (id, x, y, dx, dy, pressure)
            if love.touchreleased then
                return love.touchreleased(id, x, y, dx, dy, pressure)
            end
        end,
        touchmoved = function (id, x, y, dx, dy, pressure)
            if love.touchmoved then
                return love.touchmoved(id, x, y, dx, dy, pressure)
            end
        end,
        joystickpressed = function (joystick, button)
            if love.joystickpressed then
                return love.joystickpressed(joystick, button)
            end
        end,
        joystickreleased = function (joystick, button)
            if love.joystickreleased then
                return love.joystickreleased(joystick, button)
            end
        end,
        joystickaxis = function (joystick, axis, value)
            if love.joystickaxis then
                return love.joystickaxis(joystick, axis, value)
            end
        end,
        -- REGION UNIMPLEMENTED --
        joystickhat = function (joystick, hat, value)
            if love.joystickhat then
                return love.joystickhat(joystick, hat, value)
            end
        end,
        -- END REGION UNIMPLEMENTED --
        gamepadpressed = function (joystick, button)
            if love.gamepadpressed then
                return love.gamepadpressed(joystick, button)
            end
        end,
        gamepadreleased = function (joystick, button)
            if love.gamepadreleased then
                return love.gamepadreleased(joystick, button)
            end
        end,
        gamepadaxis = function (joystick, axis, value)
            if love.gamepadaxis then
                return love.gamepadaxis(joystick, axis, value)
            end
        end,
        joystickadded = function (joystick)
            if love.joystickadded then
                return love.joystickadded(joystick)
            end
        end,
        joystickremoved = function (joystick)
            if love.joystickremoved then
                return love.joystickremoved(joystick)
            end
        end,
        focus = function (focused)
            if love.focus then
                return love.focus(focused)
            end
        end,
        -- REGION UNUSED --
        mousefocus = function (mouseFocused)
            if love.mousefocus then
                return love.mousefocus(mouseFocused)
            end
        end,
        -- END REGION UNUSED --
        visible = function (visible)
            if love.visible then
                return love.visible(visible)
            end
        end,
        quit = function ()
            return
        end,
        threaderror = function (thread, error)
            if love.threaderror then
                return love.threaderror(thread, error)
            end
        end,
        resize = function (width, height)
            if love.resize then
                return love.resize(width, height)
            end
        end,
        -- REGION UNUSED --
        filedropped = function (file)
            if love.filedropped then
                return love.filedropped(file)
            end
        end,
        directorydropped = function (directory)
            if love.directorydropped then
                return love.directorydropped(directory)
            end
        end,
        -- END REGION UNUSED --
        lowmemory = function ()
            if love.lowmemory then
                love.lowmemory()
            end
            collectgarbage()
            collectgarbage()
        end,
        displayrotated = function (display, orient)
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

local function saveError(text)
    if not love.filesystem then
        return
    end

    love.filesystem.createDirectory("errors")

    local date = os.date("%H%M%S_%m%d%y")
    local filename = string.format("errors/love_error_%s.txt", date)

    love.filesystem.write(filename, text)

    return filename
end

local function hackForMissingFilename(error)
    -- assume that all missing filenames which are required
    -- use this in their error message
    if not error:find("module") then
        return error
    end

    local split = {}

    -- split by newlines
    for line in error:gmatch("(.-)\n") do
        local value = line:gsub("'", "")
        if value:sub(-3) ~= ".so" and not value:find("/usr/") then
            table.insert(split, line)
        end
    end

    -- return our new string
    return table.concat(split, "\n")
end

local function is3DHack()
    if love._console_name == "3DS" then
        return love.graphics.get3D()
    end
    return true
end

function love.run()
    if love.load then
        love.load(arg)
    end

    if love.timer then
        love.timer.step()
    end

    local delta = 0

    local normalScreens = love.graphics.getScreens()
    local plainScreens
    if love._console_name == "3DS" then
        plainScreens = {"top", "bottom"}
    end

    return function()
        if love.window and g_windowShown then
            return
        end

        if love.event and love.event.pump then
            love.event.pump()

            for name, a, b, c, d, e, f in love.event.poll() do
                if name == "quit" then
                    if not love.quit or not love.quit() then
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
            local screens = is3DHack() and normalScreens or plainScreens

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
    error("Thread error ("..tostring(t)..")\n\n".. err, 0)
end

local utf8 = require("utf8")

local function error_printer(msg, layer)
    print((debug.traceback("Error: " .. tostring(msg), 1 + (layer or 1)):gsub("\n[^\n]+$", "")))
end

local max_lines = 14
local function fix_long_error(font, text, max_width)
    local text_result, result = "", 0

    for line in text:gmatch("(.-)\n") do
        if result < max_lines then
            local width = 10
            for index = 1, #line do
                local glyph = line:sub(index, index)
                width = width + font:getWidth(glyph)
                if width > max_width then
                    text_result = text_result .. "\n"
                    result = result + 1
                    width = 10
                end
                text_result = text_result .. glyph
            end
            text_result = text_result .. "\n"
        end
        result = result + 1
    end

    -- note: don't forget to concat this
    local str = "%s\n... and %d more lines."
    local extra_lines = (result - max_lines)
    local full_text = str:format(text_result, extra_lines)

    return full_text
end

function love.errorhandler(message)
    message = tostring(message)

    error_printer(message, 2)

    if not love.window or not love.event then
        return
    end

    if not love.window.isOpen() then
        local success, status = pcall(love.window.setMode)
        if not success or not status then
            return
        end
    end

    if love.joystick then
        for _, v in ipairs(love.joystick.getJoysticks()) do
            v:setVibration()
        end
    end

    if love.audio then
        love.audio.stop()
    end

    love.graphics.reset()

    local fontSize = 12
    if love._console_name == "Switch" then
        fontSize = 24
    end

    love.graphics.origin()

    local font = love.graphics.setNewFont(fontSize)

    love.graphics.setColor(1, 1, 1, 1)

    local trace = debug.traceback()

    -- love.graphics.origin

    local sanitized = {}
    for char in message:gmatch(utf8.charpattern) do
        table.insert(sanitized, char)
    end
    sanitized = table.concat(sanitized)

    local err = {}

    table.insert(err, "Error\n")
    table.insert(err, sanitized)

    if #sanitized ~= #message then
        table.insert(err, "Invalid UTF-8 string in error message.\n")
    end

    for line in trace:gmatch("(.-)\n") do
        if not line:match("boot.lua") then
            line = line:gsub("stack traceback:", "\nTraceback\n")
            table.insert(err, line)
        end
    end

    local pretty = table.concat(err, "\n")

    pretty = pretty:gsub("\t", "    ")
    pretty = pretty:gsub("%[string \"(.-)\"%]", "%1")

    local screen = "left"
    if love._console_name == "Switch" then
        screen = nil
    end
    local pretty_fixed = fix_long_error(font, pretty, love.graphics.getWidth(screen) * 0.75)

    -- tell the user about how to quit the error handler
    pretty_fixed = pretty_fixed .. "\n\nPress A to save this error or Start to quit.\n"

    if not love.window.isOpen() then
        return
    end

    local normalScreens = love.graphics.getScreens()
    local plainScreens
    if love._console_name == "3DS" then
        plainScreens = {"top", "bottom"}
    end

    local function draw()
        if love.graphics then
            local screens = is3DHack() and normalScreens or plainScreens

            for _, screen in ipairs(screens) do
                love.graphics.origin()

                love.graphics.setActiveScreen(screen)
                love.graphics.clear(0.35, 0.62, 0.86)

                if screen ~= "bottom" then
                    local line_num = 1
                    for line in pretty_fixed:gmatch("(.-)\n") do
                        love.graphics.print(line, 10, 5 + (line_num - 1) * font:getHeight())
                        line_num = line_num + 1
                    end
                end
            end

            love.graphics.present()
        end
    end

    local fullErrorText = pretty
    local saved = false

    local function updateError()
        if not saved then
            local filename = saveError(fullErrorText)
            pretty_fixed = pretty_fixed .. "Saved to " .. filename .. "!\n"

            draw()
            saved = true
        end
    end

    return function()
        if love.event then
            love.event.pump()

            for name, a, b, c, d, e, f in love.event.poll() do
                if name == "quit" then
                    return 1
                elseif name == "gamepadpressed" then
                    if b == "start" then
                        return 1
                    elseif b == "a" then
                        updateError()
                    end
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
