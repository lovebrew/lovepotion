-- The main boot script.
-- This code is licensed under the MIT Open Source License.

-- Copyright (c) 2018-2020
--    Jeremy S. Postelnek - jeremy.postelnek@gmail.com
--    Logan Hickok-Dickson - notquiteapex@gmail.com
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

-- make sure love exists
local love = require("love")

love.path = {}
love.arg = {}

-- Replace any \ with /.
function love.path.normalslashes(p)
    return p:gsub("\\", "/")
end

-- Makes sure there is a slash at the end
-- of a path.
function love.path.endslash(p)
    if p:sub(-1) ~= "/" then
        return p .. "/"
    else
        return p
    end
end

-- Checks whether a path is absolute or not.
function love.path.abs(p)

    local tmp = love.path.normalslashes(p)

    -- Path is absolute if it starts with a "/".
    if tmp:find("/") == 1 then
        return true
    end

    -- Path is absolute if it starts with a
    -- letter followed by a colon.
    if tmp:find("%a:") == 1 then
        return true
    end

    -- Relative.
    return false

end

-- Returns the leaf of a full path.
function love.path.leaf(p)
    p = love.path.normalslashes(p)

    local a = 1
    local last = p

    while a do
        a = p:find("/", a + 1)

        if a then
            last = p:sub(a + 1)
        end
    end

    return last
end

-- Converts any path into a full path.
function love.path.getFull(p)

    if love.path.abs(p) then
        return love.path.normalslashes(p)
    end

    local cwd = love.filesystem.getWorkingDirectory()
    cwd = love.path.normalslashes(cwd)
    cwd = love.path.endslash(cwd)

    -- Construct a full path.
    local full = cwd .. love.path.normalslashes(p)

    -- Remove trailing /., if applicable
    return full:match("(.-)/%.$") or full
end

-- Finds the key in the table with the lowest integral index. The lowest
-- will typically the executable, for instance "lua5.1.exe".
function love.arg.getLow(a)
    local m = math.huge

    for key, value in pairs(a) do
        if key < m then
            m = key
        end
    end

    return a[m], m
end


love.arg.options =
{
    console = { a = 0 },
    fused   = { a = 0 },
    game    = { a = 1 }
}

love.arg.optionIndices = {}

function love.arg.parseOption(m, i)
    m.set = true

    if m.a > 0 then
        m.arg = {}
        for j = i, i + m.a - 1 do
            love.arg.optionIndices[j] = true
            table.insert(m.arg, arg[j])
        end
    end

    return m.a
end

function love.arg.parseOptions()
    local game
    local argc = #arg

    local i = 1
    while i <= argc do
        -- Look for options.
        local m = arg[i]:match("^%-%-(.*)")

        if m and m ~= "" and love.arg.options[m] and not love.arg.options[m].set then
            love.arg.optionIndices[i] = true
            i = i + love.arg.parseOption(love.arg.options[m], i + 1)
        elseif m == "" then -- handle '--' as an option
            love.arg.optionIndices[i] = true
            if not game then -- handle '--' followed by game name
                game = i + 1
            end
            break
        elseif not game then
            game = i
        end
        i = i + 1
    end

    -- checks argv that was parsed
    if not love.arg.options.game.set then
        if game then -- then we have argv[1] -- file association game
            love.arg.parseOption(love.arg.options.game, game or 0)
        else -- enforce the game folder to be checked
            love.arg.options.game.arg = {"./game"}
            love.arg.options.game.set = true
        end
    end
end

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
        mousemoved = function (x, y, dx, dy, t)
            if love.mousemoved then
                return love.mousemoved(x, y, dx, dy, t)
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
        touchpressed = function(id, x, y, dx, dy, pressure)
            if love.touchpressed then
                return love.touchpressed(id, x, y, pressure)
            end
        end,
        touchreleased = function(id, x, y, dx, dy, pressure)
            if love.touchreleased then
                return love.touchreleased(id, x, y, pressure)
            end
        end,
        touchmoved = function(id, x, y, dx, dy, pressure)
            if love.touchmoved then
                return love.touchmoved(id, x, y, pressure)
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
            return
        end,
        threaderror = function (t, err)
            if love.threaderror then return love.threaderror(t, err) end
        end,
        lowmemory = function ()
            if love.lowmemory then love.lowmemory() end
            collectgarbage()
            collectgarbage()
        end
    }, {
        __index = function(self, name)
            error('Unknown event: ' .. tostring(name))
        end,
    })
end

local utf8 = require("utf8")
local debug, print, error = debug, print, error

local function error_printer(msg, layer)
    trace = debug.traceback("Error: " .. tostring(msg), 1 + (layer or 1))
    trace = trace:gsub("\n[^\n]+$", "")

    print(trace)
end

function love.threaderror(t, err)
    error("Thread error (".. tostring(t) ..")\n\n".. err, 0)
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
        for i, v in ipairs(love.joystick.getJoysticks()) do
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
        table.insert(err, "Invalid UTF-8 string in error message.")
    end

    table.insert(err, "\n")

    for l in trace:gmatch("(.-)\n") do
        if not l:match("boot.lua") then
            l = l:gsub("stack traceback:", "Traceback\n")
            table.insert(err, l)
        end
    end

    local pretty = table.concat(err, "\n")

    pretty = pretty:gsub("\t", "")
    pretty = pretty:gsub("%[string \"(.-)\"%]", "%1")

    if not love.window.isOpen() then
        return
    end

    local screens = love.graphics.getScreens()

    local function draw()
        if love.graphics then
            for _, screen in ipairs(screens) do
                love.graphics.setActiveScreen(screen)
                love.graphics.clear(0.35, 0.62, 0.86)

                if screen ~= "bottom" then
                    love.graphics.printf(pretty, 10, 10, love.graphics.getWidth() * 0.75)
                end
            end

            love.graphics.present()
        end
    end

    return function()
        if love.event then
            love.event.pump()

            for name, a, b, c, d, e, f in love.event.poll() do
                if name == "quit" then
                    return 1
                elseif name == "gamepadpressed" and b == "start" then
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

local no_game_code = false
local invalid_game_path = nil
local can_no_game = false

function love.boot()
    -- Load the LOVE filesystem module, its absolutely needed
    require("love.filesystem")

    local arg0 = love.arg.getLow(arg)

    love.filesystem.init(arg0)

    local exepath = love.filesystem.getExecutablePath()

    -- This shouldn't happen, but
    -- just in case we'll fall back to arg0.
    if #exepath == 0 then
        exepath = arg0
    end

    local can_has_game = pcall(love.filesystem.setSource, exepath)

    -- It's a fused game, don't parse --game argument
    if can_has_game then
        love.arg.options.game.set = true
    end

    -- Parse options now that we know which options we're looking for.
    love.arg.parseOptions()
    local o = love.arg.options

    local identity = ""
    if not can_has_game and o.game.set and o.game.arg[1] then
        local directory = o.game.arg[1]

        local fullSauce

        -- argv[1] has the full path to the .love
        if not directory:find("(%w.love)") then
            fullSauce = love.path.getFull(directory)
        else -- raw game directory
            fullSauce = directory
        end

        can_has_game = pcall(love.filesystem.setSource, fullSauce)

        if not can_has_game then
            invalid_game_path = fullSauce
        end

        identity = love.path.leaf(fullSauce)
    else
        identity = love.path.leaf(exepath)
    end

    -- Try to use the archive containing main.lua as the identity name. It
    -- might not be available, in which case the fallbacks above are used.
    local realdir = love.filesystem.getRealDirectory("main.lua")

    if realdir then
        identity = love.path.leaf(realdir)
    end

    identity = identity:gsub("^([%.]+)", "") -- strip leading "."'s
    identity = identity:gsub("%.([^%.]+)$", "") -- strip extension
    identity = identity:gsub("%.", "_") -- replace remaining "."'s with "_"
    identity = #identity > 0 and identity or "game"

    pcall(love.filesystem.setIdentity, identity, true)

    if can_has_game and not (love.filesystem.getInfo("main.lua") or love.filesystem.getInfo("conf.lua")) then
        no_game_code = true
    end

    if not can_has_game then
        can_no_game = pcall(love.filesystem.setSource, "romfs:/")
        love.filesystem.setFused(true)
    end
end

function love.init()
    local config =
    {
        identity = false,
        appendidentity = false,
        version = love._version,
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

        -- Anything from here after in the config
        -- is only included for legacy purposes

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

    -- We can't throw any errors just yet because we want to see if we can
    -- load and use conf.lua in case the user doesn't want to use certain
    -- modules, but we also can't error because graphics haven't been loaded.
    local confok, conferr
    if (not love.conf) and love.filesystem and love.filesystem.getInfo("conf.lua") then
        confok, conferr = pcall(require, "conf")
    end

    if love.conf then
        confok, conferr = pcall(love.conf, config)
    end

    local openedConsole = false
    if config.console and love._openConsole and not openedConsole then
        openedConsole = love._openConsole()
    end

    if love._setAccelerometerAsJoystick then
        love._setAccelerometerAsJoystick(config.accelerometerjoystick)
    end

    -- Modules to load
    local modules =
    {
        "data",
        "thread",
        "timer",
        "event",
        "font",
        "keyboard",
        "joystick",
        "touch",
        "sound",
        "system",
        "audio",
        "window",
        "graphics",
        "math"
    }

    -- Load them all!
    for i, v in ipairs(modules) do
        if config.modules[v] then
            pcall(function() require("love." .. v) end)
        end
    end

    if love.event then
        love.createhandlers()
    end

    -- Now we can throw any errors from `conf.lua`.
    if not confok and conferr then
        error(conferr)
    end

    -- Set up the window
    if config.window and config.modules.window then
        assert(love.window.setMode(), "Could not set window mode")
    end

    -- Take our first step.
    -- Window creation can take some time™
    if love.timer then
        love.timer.step()
    end

    if love.filesystem then
        love.filesystem.setIdentity(config.identity or love.filesystem.getIdentity(), config.appendidentity)

        if love.filesystem.getInfo("main.lua") then
            require("main")
        end
    end

    if can_no_game then
        return
    end

    if no_game_code then
        error("No code to run. Your game might be packaged incorrectly. Make sure main.lua is at the top level of the ROMFS.")
    elseif invalid_game_path then
        error("Cannot load game at path '" .. invalid_game_path .. "'. Make sure a folder exists at the specified path.")
    end
end

function love.run()
    if love.load then
        love.load(arg)
    end

    if love.timer then
        love.timer.step()
    end

    local delta = 0
    local screens = love.graphics.getScreens()
    print(unpack(screens))

    return function()
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

        if love.graphics then
            love.graphics.origin()

            for _, screen in ipairs(screens) do
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

return function()
    local func
    local inerror = false

    local function deferErrhand(...)
        local errhand = love.errhandler or love.errhand
        local handler = (not inerror and errhand) or error_printer

        inerror = true
        func = handler(...)
    end

    local function earlyInit()
        local result = xpcall(love.boot, error_printer)

        -- quit immediately
        if not result then
            return 1
        end

        result = xpcall(love.init, deferErrhand)

        -- If love.init or love.run fails, don't return a value,
        -- as we want the error handler to take over
        if not result then
            return
        end

        result = xpcall(love._ensureApplicationType, deferErrhand)

        -- let errhand take over
        if not result then
            return
        end

        local main
        result, main = xpcall(love.run, deferErrhand)

        if result then
            func = main
        end
    end

    func = earlyInit

    while func do
        local _, retval = xpcall(func, deferErrhand)

        if retval then
            return retval
        end

        coroutine.yield()
    end

    return 1
end
