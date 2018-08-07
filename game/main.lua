local N3DS = love.graphics.newImage("graphics/screen.png")

local eyeImage = love.graphics.newImage("graphics/eye.png")
local eyeQuads = {}
for x = 1, 4 do
    eyeQuads[x] = love.graphics.newQuad((x - 1) * 37, 0, 37, 37, eyeImage:getWidth(), eyeImage:getHeight())
end

local text = "nogame"
local textImage = love.graphics.newImage("graphics/nogame.png")
local textQuads = {}
for i = 1, #text do
    textQuads[text:sub(i, i)] = love.graphics.newQuad((i - 1) * 70, 0, 70, 63, textImage:getWidth(), textImage:getHeight())
end

local eyeDelay = math.random(1, 3)

function newEye(x, y)
    local eye = {}

    eye.x = x
    eye.y = y

    eye.timer = 0
    eye.quadi = 1

    eye.animation = {1, 2, 3, 4, 3, 2, 1}

    function eye:update(dt)
        if eyeDelay <= 0 then
            self.timer = self.timer + 8 * dt

            local index = math.floor(self.timer % #self.animation) + 1
            if index ~= #self.animation then
                self.quadi = self.animation[index]
            else
                eyeDelay = math.random(1, 3)
                self.quadi = 1
            end
        end
    end

    function eye:draw()
        love.graphics.draw(eyeImage, eyeQuads[self.quadi], self.x, self.y)
    end

    return eye
end

function love.load()
    leftEye = newEye(130, 87)
    rightEye = newEye(236, 87)

    love.graphics.setBackgroundColor(0.227, 0.443, 0.792)
end

function love.update(dt)
    eyeDelay = math.max(eyeDelay - dt, 0)

    leftEye:update(dt)
    rightEye:update(dt)
end

function love.draw()
    love.graphics.setScreen("top")

    love.graphics.draw(N3DS, (love.graphics.getWidth() - N3DS:getWidth()) / 2, (love.graphics.getHeight() - N3DS:getHeight()) / 2)

    leftEye:draw()
    rightEye:draw()

    love.graphics.setScreen("bottom")

    local add = math.sin(love.timer.getTime() * 2) + 4
    love.graphics.draw(textImage, textQuads["n"], 90, 50 + add)
    love.graphics.draw(textImage, textQuads["o"], 160, 50 - add)

    love.graphics.draw(textImage, textQuads["g"], 20, 140 + add)
    love.graphics.draw(textImage, textQuads["a"], 90, 140 - add)
    love.graphics.draw(textImage, textQuads["m"], 160, 140 + add)
    love.graphics.draw(textImage, textQuads["e"], 235, 140 - add)
end

require 'Horizon'