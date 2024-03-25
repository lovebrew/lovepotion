R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.
---@class love
---@class love
local love = require("love")

---The nogame ( ͡° ͜ʖ ͡°)
function love.nogame()
    ---For testing on PC, use nest
    if love.filesystem.getInfo("nest/init.lua", "file") then
        require("nest").init({ console = "switch" })
    end

    ---Temporary locals for the strings to set
    local no_game_path, cartridge_path = nil, nil

    local operating_system = love._os:lower()
    local running_console = love._console and love._console:lower()

    local path, mount_path = "assets/", nil
    if operating_system == "windows" then
        if running_console == "3ds" then
            path = path .. "ctr/"
        end
        no_game_path = path .. "nogame.png"
        cartridge_path = path .. "cartridge.png"
    else
        mount_path = "romfs"
        if operating_system ~= "horizon" then
            path = "/vol/content/"
        else
            path = "romfs:/"
        end

        local success = love.filesystem.mountFullPath(path, mount_path, "read", true)
        
        if not success then
            error("Failed to mount nogame romfs")
        end

        no_game_path = mount_path .. "/nogame/nogame.png"
        cartridge_path = mount_path .. "/nogame/cartridge.png"
    end

    local wave_thread_code =
    [[
        local heights, phases, speeds, dimensions = ...

        love.timer = require("love.timer")
        love.math  = require("love.math")

        local step = math.ceil(dimensions.x / 100)

        if dimensions.x == 400 then
            step = 100
        elseif dimensions.x == 1280 then
            step = 128
        elseif dimensions.x == 1920 then
            step = 192
        end

        local channels =
        {
            love.thread.getChannel("wave_1"),
            love.thread.getChannel("wave_2")
        }

        local function main_loop()
            for index = 1, #channels do
                channels[index]:clear()

                -- points to render BezierCurve with
                local points = {}
                local height = dimensions.y - (dimensions.y * heights[index])

                for x = 0, dimensions.x, step do
                    local sine = math.sin(x * speeds[index] / dimensions.x + love.timer.getTime() + phases[index])
                    local y    = sine * 10 + height

                    table.insert(points, x)
                    table.insert(points, y)
                end

                local curve_points = love.math.newBezierCurve(points):render(2)

                table.insert(curve_points, dimensions.x)
                table.insert(curve_points, dimensions.y)

                table.insert(curve_points, 0)
                table.insert(curve_points, dimensions.y)

                channels[index]:supply(curve_points)
            end
        end

        while true do
            main_loop()
        end
    ]]

    local Wave = {}
    Wave.__index = Wave

    function Wave.new(id, color)
        local self   = setmetatable({}, Wave)

        self.channel = love.thread.getChannel(("wave_%d"):format(id))
        self.color   = color

        return self
    end

    function Wave:draw()
        local points = self.channel:demand()

        love.graphics.setColor(self.color)
        love.graphics.polygon("fill", points)
    end

    local NoGame = {}
    NoGame.__index = NoGame

    function NoGame.new()
        local self = setmetatable({}, NoGame)
        self.texture = love.graphics.newImage(no_game_path)

        local screen = self.getPosition()
        self.x = (love.graphics.getWidth(screen) - self.texture:getWidth()) * 0.5

        if screen == "default" then
            self.y = (love.graphics.getHeight() * 0.95) - self.texture:getHeight()
            return self
        end

        self.y = (love.graphics.getHeight() - self.texture:getHeight()) * 0.5
        return self
    end

    function NoGame.getPosition()
        if running_console == "wii u" then
            return "gamepad"
        elseif running_console == "switch" then
            return "default"
        end
        return "bottom"
    end

    function NoGame:draw()
        love.graphics.setColor(1, 1, 1, 1)
        love.graphics.draw(self.texture, self.x, self.y)
    end

    local Cartridge = {}
    Cartridge.__index = Cartridge

    function Cartridge.new()
        local self = setmetatable({}, Cartridge)

        local width, height = 0, 0
        self.texture = love.graphics.newImage(cartridge_path)

        width, height, self.quads = Cartridge.createQuads(self.texture)

        self.offset = {
            x = width * 0.5,
            y = height * 0.5
        }

        self.x = (love.graphics.getWidth() - width) * 0.5
        self.y = (love.graphics.getHeight() - height) * 0.5

        self.x = self.x + self.offset.x
        self.y = self.y + self.offset.y

        self.angle = 0
        self.isRight = true

        self.maxAngle = math.pi / 8

        return self
    end

    ---Create the quads for the cartridge
    ---@param texture love.Image
    ---@return number, number, love.Quad
    function Cartridge.createQuads(texture)
        local result = {}

        local width, height = 150, 150
        if running_console ~= "3ds" then
            width, height = 600, 600
        end

        for index = 1, 2 do
            result[index] = love.graphics.newQuad((index - 1) * width, 0, width, height, texture)
        end

        return width, height, result
    end

    function Cartridge:update(dt)
        local angle_add = -math.pi / 4
        if self.isRight then
            angle_add = -angle_add
        end
        self.angle = self.angle + angle_add * dt

        if self.angle > self.maxAngle then
            self.isRight = false
            return
        end

        if self.angle < -self.maxAngle then
            self.isRight = true
        end
    end

    function Cartridge:draw(depth)
        love.graphics.setColor(1, 1, 1, 1)

        local x_depth = self.x - (depth * 3)

        for index = 1, #self.quads do
            local quad = self.quads[index]
            love.graphics.draw(self.texture, quad, x_depth, self.y, self.angle, 1, 1, self.offset.x, self.offset.y)
        end
    end

    local waves = {}

    local no_game     = nil
    local cartridge   = nil
    local wave_thread = nil

    function love.load()
        love.graphics.setBackgroundColor(0.93, 0.93, 0.93)

        table.insert(waves, Wave.new(1, { 0.81, 0.31, 0.56 }))
        table.insert(waves, Wave.new(2, { 0.28, 0.65, 0.93 }))

        wave_thread = love.thread.newThread(wave_thread_code)

        cartridge = Cartridge.new()
        no_game   = NoGame.new()

        local width, height = love.graphics.getDimensions()
        wave_thread:start({ 0.35, 0.25 }, { 0, 2 }, { 3.0, 3.5 }, { x = width, y = height })
    end

    function love.update(dt)
        cartridge:update(dt)
    end

    local function draw_top(depth)
        for _, value in ipairs(waves) do
            value:draw()
        end

        cartridge:draw(depth)

        if running_console == "switch" then
            no_game:draw()
        end
    end

    local function draw_bottom(screen)
        if running_console ~= "switch" then
            love.graphics.setColor(waves[2].color)
            love.graphics.rectangle("fill", 0, 0, love.graphics.getDimensions(screen))
        end

        no_game:draw()
    end

    local screens = { "left", "right", "tv", "default" }

    local function contains(t, value)
        for key, needle in ipairs(t) do
            if needle == value then
                return true
            end
        end
        return false
    end

    ---Gets the 3D Depth on the 3DS
    ---@return number
    local function get_depth()
        if love.graphics.getDepth then
            return love.graphics.getDepth()
        end
        return 0
    end

    function love.draw(screen)
        local depth = -get_depth()
        if screen == "right" then
            depth = -depth
        end

        if contains(screens, screen) then
            draw_top(depth)
        else
            draw_bottom(screen)
        end
    end

    function love.gamepadpressed(_, button)
        if button == "start" then
            love.event.quit()
        end
    end
end

return love.nogame
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
