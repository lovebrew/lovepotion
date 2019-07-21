-- This code is licensed under the MIT Open Source License.
--[[
    Copyright (c) 2019 Jeremy S. Postelnek - jeremy.postelnek@gmail.com
    Copyright (c) 2019 Logan Hickok-Dickson - notquiteapex@gmail.com
    Copyright (c) 2016 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
--]]

-- Load the LOVE filesystem module, its absolutely needed
love.filesystem = require("love.filesystem")

-- 3DS and Switch can't load external C libraries,
-- so we need to prevent require from searching for them
package.path = './?.lua;./?/init.lua'
package.cpath = './?.lua;./?/init.lua'

-- Base game configuration
local config =
{
    identity = "SuperGame",
    appendidentity = false,
    version = "1.1.0",
    console = false,
    
    modules =
    {
        audio = true,
        data = true,
        event = true,
        font = true,
        graphics = true,
        image = true,
        joystick = true,
        keyboard = true,
        math = true,
        mouse = true,
        physics = true,
        sound = true,
        system = true,
        thread = true,
        timer = true,
        touch = true,
        video = true,
        window = true
    },

    -- Anything from here after in the config is only included for legacy purposes

    accelerometerjoystick = true,
    externalstorage = false,

    gammacorrect = true,

    audio =
    {
        mixwithsystem = true
    },

    window =
    {
        title = "Untitled",
        icon = nil,

        width = 1280,
        height = 720,

        borderless = false,
        resizable = false,

        minwidth = 1,
        minheight = 1,

        fullscreen = false,
        fullscreentype = "desktop",

        vsync = 1,
        msaa = 0,

        depth = nil,
        stencil = nil,

        highdpi = false,

        x = nil,
        y = nil,
    }
}

-- Standard event callback handlers.
function love.createhandlers()
    love.handlers = setmetatable({
        keypressed = function (key)
            if love.keypressed then
                return love.keypressed(key)
            end
        end,
        keyreleased = function (key)
            if love.keyreleased then
                return love.keyreleased(key)
            end
        end,
        mousemoved = function (x,y,dx,dy,t)
            if love.mousemoved then
                return love.mousemoved(x,y,dx,dy,t)
            end
        end,
        mousepressed = function (x, y, button)
            if love.mousepressed then
                return love.mousepressed(x, y, button)
            end
        end,
        mousereleased = function (x, y, button)
            if love.mousereleased then
                return love.mousereleased(x, y, button)
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
        joystickhat = function (joystick, hat, value)
            if love.joystickhat then
                return love.joystickhat(joystick, hat, value)
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
        textinput = function(text)
            if love.textinput then
                return love.textinput(text)
            end
        end,
        focus = function (focus)
            if love.focus then
                return love.focus(focus)
            end
        end,
        visible = function (visible)
            if love.visible then
                return love.visible(visible)
            end
        end,
        quit = function ()
            if love.quit then
                return love.quit()
            end
        end,
        threaderror = function (t, err)
            if love.threaderror then return love.threaderror(t, err) end
        end,
        lowmemory = function ()
            collectgarbage()
            if love.lowmemory then return love.lowmemory() end
        end
    }, {
        __index = function(self, name)
            error('Unknown event: ' .. name)
        end,
    })
end

function love.errorhandler(message)
    message = tostring(message)

    message = message:gsub("^(./)", "")

    local err = {}

    local major, minor, rev = love.getVersion()

    table.insert(err, message .. "\n")

    if love.audio then
        love.audio.stop()
    end

    local trace = debug.traceback()

    for l in trace:gmatch("(.-)\n") do
        if not l:match("boot.lua") then
            l = l:gsub("stack traceback:", "Traceback:\n")
            table.insert(err, l)
        end
    end

    local realError = table.concat(err, "\n")
    realError = realError:gsub("\t", "")
    realError = realError:gsub("%[string \"(.-)\"%]", "%1")

    local copy = err
    table.insert(copy, "\nLove Potion " .. love.getVersion(true) .. " (API " .. major .. "." .. minor .. "." .. rev .. ")")

    local dateTime = os.date("%c")
    table.insert(copy, "\nDate and Time: " .. dateTime)
    table.insert(copy, "\nA log has been saved to " .. love.filesystem.getSaveDirectory() .. "LoveCrash.txt")

    local fullError = table.concat(err, "\n")
    love.filesystem.write("LoveCrash.txt", fullError)

    love.graphics.setBackgroundColor(0.35, 0.62, 0.86)

    local headerSize, buttonSize = 30, 15
    if love._os[2] == "Switch" then
        headerFont, buttonSize = 96, 24
    end

    local headerFont = love.graphics.newFont(headerSize)
    local buttonFont = love.graphics.newFont(buttonSize)

    local error_img, start_img
    
    if love._os[2] == "3DS" then
        error_img = love.graphics.newImage("error:warn_sm")
        start_img = love.graphics.newImage("error:button_sm")
    elseif love._os[2] == "Switch" then
        error_img = love.graphics.newImage("error:warn")
        start_img = love.graphics.newImage("error:button")
    end

    local width = love.graphics.getWidth()
    local height = love.graphics.getHeight()

    love.draw = function()
        love.graphics.origin()
        love.graphics.clear()

        love.graphics.draw(error_img, 40, (height * 0.15) / 2 - error_img:getHeight() / 2)

        love.graphics.setColor(1, 1, 1, 1)

        love.graphics.setFont(headerFont)
        love.graphics.print("Lua Error", 40 + error_img:getWidth() + 16, ((height * 0.15) / 2 - headerFont:getHeight() / 2))

        love.graphics.rectangle("fill", 30, height * 0.15, width - 60, 2)

        love.graphics.setFont(buttonFont)
        love.graphics.print(realError, 40, height * 0.20)

        love.graphics.rectangle("fill", 30, height * 0.90, width - 60, 2)

        love.graphics.draw(start_img, width * 0.75, (height * 0.95) - start_img:getHeight() / 2)
        love.graphics.print("Quit", (width * 0.75) + start_img:getWidth() + 12, (height * 0.95) - buttonFont:getHeight() / 2)

        love.graphics.present()

        if love._os[2] == "3DS" then
            love.graphics.clear("bottom")
            love.graphics.present()
        end
    end

    local quit_string = "start"
    if love._os[2] == "Switch" then
        quit_string = "plus"
    end

    love.gamepadpressed = function(joy, button)
        if button == quit_string then
            love.event.quit()
        end
    end
end

-- love.errhand was deprecated in LOVE 11.0, replaced by love.errorhandler.
love.errhand = love.errorhandler

function love.threaderror(t, err)
	error("Thread error ("..tostring(t)..")\n\n"..err, 0)
end

--################--
-- BOOT THE GAME! --
--################--

function love.boot()
    -- We can't throw any errors just yet because we want to see if we can
    -- load and use conf.lua in case the user doesn't want to use certain
    -- modules, but we also can't error because graphics haven't been loaded.
    local confok, conferr

    if love.filesystem.getInfo("conf.lua", "file") then
        confok, conferr = pcall(require, "conf")

        if confok and love.conf then
            confok, conferr = pcall(love.conf, config)
        end
    end

    -- Set the game identity for saving.
    love.filesystem.setIdentity(config.identity)

    -- Load the modules.
    local modules =
    {
        -- Try to load love.graphics, window, and event first in case we need
        -- to jump to errhand after it. (window is only required on Switch.)
        "graphics",
        "window",
        "event",
        "timer",
        "joystick",
        "audio",
        "keyboard",
        "math",
        "system",
    }
    for i, v in ipairs(modules) do
        if config.modules[v] then
            love[v] = require("love." .. v)
        end
    end

    -- Load any Switch exclusive modules.
    if love._os[2] == "Switch" then
        local modulesNX =
        {
            "image",
            "thread",
            "touch",
        }
        for i, v in ipairs(modulesNX) do
            if config.modules[v] then
                love[v] = require("love." .. v)
            end
        end
    end

    if love.event then
        love.createhandlers()
    end
    
    -- Take our first step.
    if love.timer then
        love.timer.step()
    end

    -- Now we can throw any errors from `conf.lua`.
    if not confok and conferr then
        error(conferr)
    end

    if love.filesystem.getInfo("main.lua", "file") then
        -- Try to load `main.lua`.
        require("main")

        -- See if loading exists and works.
        if love.load then
            love.load()
        end
    else
        -- If there's no game to load then we'll just load up something on the
        -- screen to tell the user that there's NO GAME!
        love._nogame()
    end
end

-- Boot up the game!
xpcall(love.boot, love.errhand)
-- If something went wrong, the errhand redefines the love.update and love.draw
-- functions which are managed by the love.run function.

-- love.run is handled in `main.cpp`.
