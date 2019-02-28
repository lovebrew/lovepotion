local N3DS = love.graphics.newImage("nogame:screen")
local bottom = love.graphics.newImage("nogame:bottom")

local eyeImage = love.graphics.newImage("nogame:eye")
local eyeQuads = {}
for x = 1, 4 do
    eyeQuads[x] = love.graphics.newQuad((x - 1) * 37, 0, 37, 37, eyeImage:getWidth(), eyeImage:getHeight())
end

local text = "nogame"
local textImage = love.graphics.newImage("nogame:nogame")
local textQuads = {}
for i = 1, #text do
    textQuads[text:sub(i, i)] = love.graphics.newQuad((i - 1) * 70, 0, 70, 63, textImage:getWidth(), textImage:getHeight())
end

local eyes = 
{
    delay = math.random(1, 3),
    quadi = 1,
    animation = {1, 2, 3, 4, 3, 2, 1},
    timer = 0,
    rate = 16
}

function newEye(x, y)
    local eye = {}

    eye.x = x
    eye.y = y

    function eye:draw()
        love.graphics.draw(eyeImage, eyeQuads[eyes.quadi], self.x, self.y)
    end

    return eye
end

leftEye = newEye(130, 87)
rightEye = newEye(236, 87)

love.graphics.setBackgroundColor(0.227, 0.443, 0.792)

function love.update(dt)
    eyes.delay = math.max(eyes.delay - dt, 0)

    if eyes.delay <= 0 then
        eyes.timer = eyes.timer + eyes.rate * dt

        local index = math.floor(eyes.timer % #eyes.animation) + 1
        if index ~= #eyes.animation then
            eyes.quadi = eyes.animation[index]
        else
            eyes.delay = math.random(1, 3)
            eyes.quadi = 1
        end
    end
end

function love.draw()
    love.graphics.setScreen("top")

    love.graphics.draw(N3DS, (love.graphics.getWidth() - N3DS:getWidth()) / 2, (love.graphics.getHeight() - N3DS:getHeight()) / 2)

    leftEye:draw()
    rightEye:draw()

    love.graphics.setScreen("bottom")

    love.graphics.draw(bottom)

    local add = math.sin(love.timer.getTime() * 2) + 4
    love.graphics.draw(textImage, textQuads["n"], 90, 50 + add)
    love.graphics.draw(textImage, textQuads["o"], 160, 50 - add)

    love.graphics.draw(textImage, textQuads["g"], 20, 140 + add)
    love.graphics.draw(textImage, textQuads["a"], 90, 140 - add)
    love.graphics.draw(textImage, textQuads["m"], 160, 140 + add)
    love.graphics.draw(textImage, textQuads["e"], 235, 140 - add)
end

function love.gamepadpressed(joy, button)
    if button == "start" then
        love.event.quit()
    end
end