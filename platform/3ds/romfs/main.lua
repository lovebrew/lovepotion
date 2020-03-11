require 'libraries.nest'.init('ctr', {no_override=true})

local function load_assets(path)
    local ret = {}

    local abs_path = string.format("graphics/%s", path)
    local files = love.filesystem.getDirectoryItems(abs_path)

    local ext = ".png"
    if path == "ctr" then
        ext = ".t3x"
    end

    for i = 1, #files do
        local index = files[i]:gsub(ext, "")
        ret[index] = love.graphics.newImage(abs_path .. "/" .. index .. ".png")
    end

    ret.eye_quads = {}
    for i = 1, 4 do
        ret.eye_quads[i] = love.graphics.newQuad((i - 1) * 37, 0, 37, 37, ret.eye)
    end

    return ret
end

local EYE_WAIT = love.math.random(3, 5)
local function create_eye(x, y)
    local eye = {}

    eye.x = x
    eye.y = y

    eye.texture = assets.eye
    eye.quads = assets.eye_quads

    eye.timer = 0
    eye.animation = {1, 2, 3, 4, 3, 2, 1}
    eye.animationIndex = 1

    function eye:update(dt)
        if EYE_WAIT > 0 then
            return
        end

        self.timer = self.timer + 8 * dt
        local index = math.floor(self.timer % #self.animation) + 1

        if index < #self.animation then
            self.animationIndex = self.animation[index]
        else
            self.animationIndex = 1
            self.timer = 0

            return true
        end
    end

    function eye:draw()
        love.graphics.draw(self.texture, self.quads[self.animationIndex], self.x, self.y)
    end

    return eye
end

--[[
    MAIN GAME STUFF BELOW
--]]

function love.load()
    local path = "desktop"
    if love._os == "Horizon" then
        path = "ctr"
    end

    assets = load_assets(path)

    local left_x = 400 * 0.32
    local right_x = (400 * 0.68) - 37
    local y = 240 * 0.28

    eyes = {create_eye(left_x, y), create_eye(right_x, y)}

    love.graphics.setBackgroundColor(0.23, 0.44, 0.79)
end

function love.update(dt)
    EYE_WAIT = EYE_WAIT - dt

    local blink = 0
    for _, eye in ipairs(eyes) do
        if eye:update(dt) then
            blink = blink + 1
        end
    end

    if blink == 2 then
        EYE_WAIT = love.math.random(3, 5)
    end
end

function love.draw(screen)
    love.graphics.draw(assets[screen])

    if screen ~= "top" then
        return
    end

    for _, v in ipairs(eyes) do
        v:draw()
    end
end

function love.gamepadpressed(joystick, button)
    if button == "start" then
        love.event.quit()
    end
end
