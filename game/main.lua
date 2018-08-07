local N3DS = love.graphics.newImage("graphics/screen.png")

local eyeImage = love.graphics.newImage("graphics/eye.png")
local eyeQuads = {}
for x = 1, 4 do
    eyeQuads[x] = love.graphics.newQuad((x - 1) * 37, 0, 37, 37, eyeImage:getWidth(), eyeImage:getHeight())
end

function newEye(x, y)
    local eye = {}

    eye.x = x
    eye.y = y

    eye.timer = 0
    eye.quadi = 1

    eye.delay = math.random(1, 3)

    eye.animation = {1, 2, 3, 4, 3, 2, 1}

    function eye:update(dt)
        if self.delay > 0 then
            self.delay = self.delay - dt
        else
            self.timer = self.timer + 8 * dt

            local index = math.floor(self.timer % #self.animation) + 1
            if index ~= #self.animation then
                self.quadi = self.animation[index]
            else
                self.delay = math.random(1, 3)
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
    leftEye:update(dt)
    rightEye:update(dt)
end

function love.draw()
    love.graphics.draw(N3DS, (love.graphics.getWidth() - N3DS:getWidth()) / 2, (love.graphics.getHeight() - N3DS:getHeight()) / 2)

    leftEye:draw()
    rightEye:draw()
end

require 'Horizon'