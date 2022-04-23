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

-- Make sure love exists.
local love = require("love")

-- Essential code boot/init.
require("love.arg")
require("love.callbacks")

local function uridecode(s)
    return s:gsub("%%%x%x", function(str)
        return string.char(tonumber(str:sub(2), 16))
    end)
end

local no_game_code = false
local invalid_game_path = nil

-- This can't be overridden.
function love.boot()

    -- This is absolutely needed.
    require("love.filesystem")

    local arg0 = love.arg.getLow(arg)
    love.filesystem.init(arg0)

    local exepath = love.filesystem.getExecutablePath()
    if #exepath == 0 then
        -- This shouldn't happen, but just in case we'll fall back to arg0.
        exepath = arg0
    end

    no_game_code = false
    invalid_game_path = nil

    -- Is this one of those fancy "fused" games?
    local can_has_game = pcall(love.filesystem.setSource, exepath)

    -- It's a fused game, don't parse --game argument
    if can_has_game then
        love.arg.options.game.set = true
    end

    -- Parse options now that we know which options we're looking for.
    love.arg.parseOptions()

    local o = love.arg.options

    local is_fused_game = can_has_game or love.arg.options.fused.set

    love.filesystem.setFused(is_fused_game)

    local identity = ""
    if not can_has_game and o.game.set and o.game.arg[1] then
        local nouri = o.game.arg[1]

        if nouri:sub(1, 7) == "file://" then
            nouri = uridecode(nouri:sub(8))
        end

        local full_source = love.path.getFull(nouri)
        can_has_game = pcall(love.filesystem.setSource, full_source)

        if not can_has_game then
            invalid_game_path = full_source
        end

        -- Use the name of the source .love as the identity for now.
        identity = love.path.leaf(full_source)
    else
        -- Use the name of the exe as the identity for now.
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

    -- When conf.lua is initially loaded, the main source should be checked
    -- before the save directory (the identity should be appended.)
    pcall(love.filesystem.setIdentity, identity, true)

    if can_has_game and not (love.filesystem.getInfo("main.lua") or love.filesystem.getInfo("conf.lua")) then
        no_game_code = true
    end

    if not can_has_game then
        local nogame = require("love.nogame")
        nogame()
    end
end

local function split(s, delim)
    local t = {}
    for text in self:gmatch("([^" .. delim .. "]+)") do
        if text then
            table.insert(t, text)
        end
    end
    return t
end

function love.init()

    -- Create default configuration settings.
    -- NOTE: Adding a new module to the modules list
    -- will NOT make it load, see below.
    local config = {
        title = "Untitled",
        version = love._version,
        potion_version = love._potion_version,
        window = {
            width = 800,
            height = 600,
            x = nil,
            y = nil,
            minwidth = 1,
            minheight = 1,
            fullscreen = false,
            fullscreentype = "desktop",
            display = 1,
            vsync = 1,
            msaa = 0,
            borderless = false,
            resizable = false,
            centered = true,
            highdpi = false,
            usedpiscale = true,
        },
        modules = {
            data = true,
            event = true,
            keyboard = true,
            mouse = false,
            timer = true,
            joystick = true,
            touch = true,
            image = true,
            graphics = true,
            audio = true,
            math = true,
            physics = true,
            sound = true,
            system = true,
            font = true,
            thread = true,
            window = true,
            video = true,
        },
        audio = {
            mixwithsystem = true,
            mic = false,
        },
        console = false,
        identity = false,
        appendidentity = false,
        externalstorage = false,
        accelerometerjoystick = true,
        gammacorrect = false,
    }

    -- If config file exists, load it and allow it to update config table.
    local confok, conferr
    if (not love.conf) and love.filesystem and love.filesystem.getInfo("conf.lua") then
        confok, conferr = pcall(require, "conf")
    end

    -- Yes, conf.lua might not exist, but there are other ways of making
    -- love.conf appear, so we should check for it anyway.
    if love.conf then
        confok, conferr = pcall(love.conf, config)
        -- If love.conf errors, we'll trigger the error after loading modules so
        -- the error message can be displayed in the window.
    end

    -- config.console is now a table or string
    -- table is { ip, port } and string is just the ip
    -- an optional third table item is a boolean to enable logging
    -- default port for nestlink is 8000
    local consoleok, consoleerr
    if config.console then
        consoleok, consoleerr = pcall(require, "love.console")

        if consoleok then
            if type(config.console) == "table" then
                consoleok, consoleerr = pcall(function()
                    love.console:init(unpack(config.console))
                end)
            elseif type(config.console) == "string" then
                consoleok, consoleerr = pcall(function()
                    local info = split(config.console, ":")
                    love.console:init(info[1], info[2])
                end)
            end
        end
    end

    if love._setAccelerometerAsJoystick then
        love._setAccelerometerAsJoystick(config.accelerometerjoystick)
    end

    if love._setGammaCorrect then
        love._setGammaCorrect(config.gammacorrect)
    end

    if love._setAudioMixWithSystem then
        if config.audio and config.audio.mixwithsystem ~= nil then
            love._setAudioMixWithSystem(config.audio.mixwithsystem)
        end
    end

    if love._requestRecordingPermission then
        love._requestRecordingPermission(config.audio and config.audio.mic)
    end

    -- Gets desired modules.
    for _, v in ipairs{
        "data",
        "thread",
        "timer",
        "event",
        "keyboard",
        "joystick",
        "touch",
        "sound",
        "system",
        "audio",
        "image",
        "font",
        "window",
        "video",
        "graphics",
        "math",
        "physics",
    } do
        if config.modules[v] then
            local success, error_msg = pcall(require, "love." .. v)
            if (not success) then
                error(error_msg)
            end
        end
    end

    if love.event then
        love.createhandlers()
    end

    -- check version - normally LÖVE's, but we
    -- want to check the LÖVE Potion version
    config._potion_version = tostring(config._potion_version)
    local  message = "This game indicates it was made for version '%s' of LÖVE Potion." ..
                     "It may not be compatible with the running version (%s)."

    if not love.isVersionCompatible(config.potion_version) then
        local major, minor, revision = config.version:match("^(%d+)%.(%d+)%.(%d+)$")
        local t = {major = love._potion_version_major, minor = love._potion_version_minor, rev = love._potion_version_revision}
        if (not major or not minor or not revision) or (major ~= t.major and minor ~= t.minor and revision ~= t.rev) then
            local formatted = message:format(config.potion_version, love._potion_version)
            print(formatted)

            if love.window then
                love.window.showMessageBox(nil, message)
            end
        end
    end

    if not confok and conferr then
        error(conferr)
    end

    if config.console and not consoleok and consoleerr then
        error(consoleerr)
    end

    -- Setup window here.
    if config.window and config.modules.window then
        assert(love.window.setMode(), "Could not set window mode")
    end

    -- Our first timestep, because window creation can take some time
    if love.timer then
        love.timer.step()
    end

    if love.filesystem then
        love.filesystem.setIdentity(config.identity or love.filesystem.getIdentity(), config.appendidentity)
        if love.filesystem.getInfo("main.lua") then
            require("main")
        end
    end

    if no_game_code then
        error("No code to run\nYour game might be packaged incorrectly.\nMake sure main.lua is at the top level of the zip.")
    elseif invalid_game_path then
		error("Cannot load game at path '" .. invalid_game_path .. "'.\nMake sure a folder exists at the specified path.")
	end
end

local print, debug, tostring = print, debug, tostring

local function error_printer(msg, layer)
    print((debug.traceback("Error: " .. tostring(msg), 1+(layer or 1)):gsub("\n[^\n]+$", "")))
end

-----------------------------------------------------------
-- The root of all calls.
-----------------------------------------------------------

return function()
    local func
    local inerror = false

    local function deferErrhand(...)
        local errhand = love.errorhandler or love.errhand
        local handler = (not inerror and errhand) or error_printer
        inerror = true
        func = handler(...)
    end

    local function earlyinit()
        -- If love.boot fails, return 1 and finish immediately
        local result = xpcall(love.boot, error_printer)
        if not result then return 1 end

        -- If love.init or love.run fails, don't return a value,
        -- as we want the error handler to take over
        result = xpcall(love.init, deferErrhand)
        if not result then return end

        -- NOTE: We can't assign to func directly, as we'd
        -- overwrite the result of deferErrhand with nil on error
        local main
        result, main = xpcall(love.run, deferErrhand)
        if result then
            func = main
        end
    end

    func = earlyinit

    while func do
        local _, retval = xpcall(func, deferErrhand)

        if retval then
            if love.console then
                love.console:close()
            end

            return retval
        end

        coroutine.yield()
    end

    return 1
end

-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
