local dogBase = love.graphics.newImage("nogame:dog")
local dogHead = love.graphics.newImage("nogame:head")
local dogTongue = love.graphics.newImage("nogame:tongue")
local headShadow = love.graphics.newImage("nogame:shadow")
local dogShadow = love.graphics.newImage("nogame:dogshadow")

local dogTail = love.graphics.newImage("nogame:tail")
local tailQuads = {}
for y = 1, 2 do
    for x = 1, 3 do
        table.insert(tailQuads, love.graphics.newQuad((x - 1) * 128, (y - 1) * 107, 128, 107, dogTail:getWidth(), dogTail:getHeight()))
    end
end
local tailTimer = 0
local tailQuadi = 1

local cloudImage = love.graphics.newImage("nogame:cloud")
local cloudQuads = {}
for i = 1, 3 do
    cloudQuads[i] = love.graphics.newQuad((i - 1) * 357, 0, 357, 190, cloudImage:getWidth(), cloudImage:getHeight())
end

love.math.setRandomSeed(os.time())
local cloudPeriods = { love.math.random(2), love.math.random(4), love.math.random(3) }

love.graphics.setBackgroundColor(0.27, 0.35, 0.39)

function love.update(dt)
    tailTimer = tailTimer + 24 * dt
    tailQuadi = math.floor(tailTimer % 6) + 1
end

function love.draw()
    love.graphics.draw(cloudImage, cloudQuads[2], love.graphics.getWidth() * 0.03, love.graphics.getHeight() * 0.15 + math.sin(love.timer.getTime() * cloudPeriods[1]) * 4)
    love.graphics.draw(cloudImage, cloudQuads[1], (love.graphics.getWidth() - 357) / 2, 0 + math.sin(love.timer.getTime() * cloudPeriods[2]) * 2)
    love.graphics.draw(cloudImage, cloudQuads[3], love.graphics.getWidth() * 0.7, love.graphics.getHeight() * 0.15 + math.sin(love.timer.getTime() * cloudPeriods[3]) * 3)


    love.graphics.draw(dogShadow)
    love.graphics.draw(dogTail, tailQuads[tailQuadi], love.graphics.getWidth() * 0.55, love.graphics.getHeight() * 0.60)
    love.graphics.draw(dogBase)
    
    local tongueOffset = math.cos(love.timer.getTime() * 12) * 3
    local headOffset = math.sin(love.timer.getTime() * 12) * 3

    love.graphics.draw(headShadow, (love.graphics.getWidth() - headShadow:getWidth()) / 2, love.graphics.getHeight() * 0.25 + headOffset)
    love.graphics.draw(dogTongue, (love.graphics.getWidth() - dogTongue:getWidth()) / 2, love.graphics.getHeight() * 0.51 + tongueOffset)
    love.graphics.draw(dogHead, (love.graphics.getWidth() - dogHead:getWidth()) / 2, love.graphics.getHeight() * 0.25 + headOffset)
end

function love.gamepadpressed(joystick, button)
    if button == "plus" then
        love.event.quit()
    end
end