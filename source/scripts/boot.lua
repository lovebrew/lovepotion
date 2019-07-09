-- This code is licensed under the MIT Open Source License.

-- Copyright (c) 2016 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:

-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.

-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

-- This is a bit messy
-- But it means we can move stuff out of main.c

package.path = './?.lua;./?/init.lua'
package.cpath = './?.lua;./?/init.lua'

--[[
    Honestly, 99% of these flags are just for compatability.
    Everything will always be enabled .. if the platform supports it.
    The only thing that matters is like the first three flags.

    Seriously: identity, appendidentity, and version.

    Go nuts.
--]]
local config =
{
    identity = "SuperGame",
    appendidentity = false,

    version = "1.0.0",
    console = false,
    accelerometerjoystick = true,
    externalstorage = false,

    gammacorrect = true,

    audio =
    {
        mixwithsystem = true
    },

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

local __defaultFont = love.graphics.newFont()
love.graphics.setFont(__defaultFont)

function love.createhandlers()
    -- Standard callback handlers.
    love.handlers = setmetatable({
        load = function()
            if love.load then
                return love.load()
            end
        end,
        update = function(dt)
            if love.update then
                return love.update(dt)
            end
        end,
        draw = function()
            if love.draw then
                return love.draw()
            end
        end,
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

love.createhandlers()

function love.errhand(message)
    message = tostring(message)

    message = message:gsub("^(./)", "")

    local err = {}

    local major, minor, rev = love.getVersion()

    table.insert(err, message .. "\n")

    love.audio.stop()

    local trace = debug.traceback()

    for l in trace:gmatch("(.-)\n") do
        if not l:match("boot.lua") then
            l = l:gsub("stack traceback:", "Traceback\n")
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
    table.insert(copy, "\nA log has been saved to " .. love.filesystem.getSaveDirectory() .. "log.txt")

    local fullError = table.concat(err, "\n")
    love.filesystem.write("log.txt", fullError)

    love.graphics.setBackgroundColor(0.35, 0.62, 0.86)

    local headerSize, buttonSize = 30, 15
    if love._os[2] == "Switch" then
        headerFont, buttonSize = 96, 24
    end

    local headerFont = love.graphics.newFont(headerSize)
    local buttonFont = love.graphics.newFont(buttonSize)

    local error_img = love.graphics.newImage("error:warn_sm");
    local start_img = love.graphics.newImage("error:button_sm");
    if love._os[2] == "Switch" then
        error_img = love.graphics.newImage("error:warn")
        start_img = love.graphics.newImage("error:button")
    end

    local width = love.graphics.getWidth()
    local height = love.graphics.getHeight()

    --local button_position = (height * 0.85) + ((height * 0.15) - start_img:getHeight() / 2) 

    local function draw()
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

    local joystick = love.joystick.getJoysticks()[1]

    local quit_string = "start"
    if love._os[2] == "Switch" then
        quit_string = "plus"
    end

    love.draw = function()
        draw()
    end

    love.gamepadpressed = function(joy, button)
        if button == quit_string then
            love.event.quit()
        end
    end
end

local function pseudoRequireConf()
    return require('conf')
end

local confSuccess
if love.filesystem.isFile("conf.lua") then
    confSuccess = xpcall(pseudoRequireConf, love.errhand)

    if not confSuccess then
        return
    end

    if love.conf then
        local function confWrap()
            love.conf(config)
        end

        confSuccess = xpcall(confWrap, love.errhand)

        if not confSuccess then
            return
        end
    end
end

love.filesystem.setIdentity(config.identity)

local function pseudoRequireMain()
    return require("main")
end

if love.filesystem.isFile("main.lua") then
    --Try main
    local result = xpcall(pseudoRequireMain, love.errhand)
    if not result then
        return
    end

    --See if loading works
    result = xpcall(love.load, love.errhand)
    if not result then
        return
    end

    --Run the thing dammit
    result = xpcall(love.run, love.errhand)
    if not result then
        return
    end
else
    local result = xpcall(love._nogame, love.errhand)
    if not result then
        return
    end
end

if love.timer then
    love.timer.step()
end