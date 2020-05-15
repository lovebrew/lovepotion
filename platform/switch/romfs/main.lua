Cloud = {}
Cloud.__index = Cloud
CloudIdx = 0

function Cloud:create(x, y, radius)
    local o = {}
    setmetatable(o, Cloud)
    o.id = CloudIdx
    o.x = x or 0
    o.y = y or 0
    o.radius = radius or 5

    ymove = 0
    xmove = 0

    xsquash = 0
    ysquash = 0

    CloudIdx = CloudIdx + 1
    return o
end

function Cloud:update()
    self.xmove = math.cos( (t + self.id/3) * 2 ) * 10;
    self.ymove = math.sin( (t + self.id/3) * 2 ) * 10

    self.xsquash = math.abs( math.cos(self.id + t) ) / 5
    self.ysquash = math.abs( math.sin(self.id + (t/2) ) ) / 5
end

function Cloud:draw()
    local ox = math.cos( (t + self.id/3) * 2 ) * 10;
    local oy = math.sin( (t + self.id/3) * 2 ) * 10;

    local xx = self.x
    local yy = self.y

    drawCircleImage(circle, xx + self.xmove, yy + self.ymove, 0.8+self.xsquash, 0.8+self.ysquash, self.radius)
end




function love.load()

    love.window.setMode(1280, 720)

    -- Load images
    img_bottleLine = love.graphics.newImage("bottleline.png")
    img_bottlefill = love.graphics.newImage("bottlefill.png")
    img_heart = love.graphics.newImage("heart.png")
    img_nogame = love.graphics.newImage("nogame.png")
    circle = love.graphics.newImage("cloudcircle.png")


    -- Config
    mainColor = hexToColor("EF808D")
    bottleTransColor = hexToColor("F8C5C7")
    
    circleSize = 128
    cloudHeight = 55
    nogameX = 640
    nogameY = 720 - 100


    -- Calculated vars
    math.randomseed(os.clock())
    winW = love.graphics.getWidth()
    winH = love.graphics.getHeight()
    centerX = winW / 2
    centerY = winH / 2
    t = 0
    circleCount = math.floor(love.graphics.getWidth() / circleSize)
    cloudYPos = love.graphics.getHeight() * (cloudHeight/100)

    -- Create clouds
    clouds = {}
    for i=0, circleCount+1 do

        local nx = lerp(0, love.graphics.getWidth(), i/(circleCount))
        local ny = cloudYPos
        local d = centerX - nx;
        local newCloud = Cloud:create(nx, ny, (circleSize/1.8) * ( 1 + math.abs(math.cos(i+d)/2)) )
        table.insert(clouds, newCloud)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    end
end

function love.update(dt)
    t = t + dt

    for i, v in ipairs(clouds) do
        v:update()
     end
end

function love.draw()

    love.graphics.setBackgroundColor( 239 / 255, 128 / 255, 141 / 255, 255 )


    love.graphics.setColor(1,1,1,1)

    ch = winH * (cloudHeight/100)

    love.graphics.rectangle("fill", 0, ch, winW, winH-ch )

    for i, cloud in ipairs(clouds) do
       cloud:draw()
    end

    love.graphics.setColor( mainColor )
    love.graphics.draw(img_nogame, nogameX, nogameY, 0, 1 + math.abs(math.cos(1-(t*2.5)))/20, 1, img_nogame:getWidth()/2, img_nogame:getHeight())

    drawBottle(1280/2, 325 + math.cos(t*5) * 15)
end

function drawCircleImage(img, x, y, xscale, yscale, radius)

    local rad = radius or 5
    local s = ((rad * 2) / img:getWidth())

    local xs = xscale or 1
    local ys = yscale or 1

    local xx = x
    local yy = y

    -- Emulate Love2d origin offset, seems to be off in LovePotion
    if love.system.getOS() == "Horizon" then
        xx = xx - img:getWidth()/2
        yy = yy - img:getHeight()/2
    end

    love.graphics.draw(img, xx, yy, 0, s * xs, s * ys, img:getWidth()/2, img:getHeight()/2)
end

function drawBottle(x, y)

    local xx
    local yy

    -- Draw Bottle Fill
    xx = x
    yy = y
    local bfW = img_bottlefill:getWidth()
    local bfH = img_bottlefill:getHeight()
    if love.system.getOS() == "Horizon" then -- Emulate Love2d origin offset, seems to be off in LovePotion
        xx = xx - bfW/2
        yy = yy - bfH/2
    end
    love.graphics.setColor( bottleTransColor )
    love.graphics.draw(img_bottlefill, xx, yy, 0, 1, 1, bfW/2, bfH/2)

    -- Draw Bottle Outline
    xx = x
    yy = y
    local boW = img_bottleLine:getWidth()
    local boH = img_bottleLine:getHeight()
    if love.system.getOS() == "Horizon" then -- Emulate Love2d origin offset, seems to be off in LovePotion
        xx = xx - bfW/2
        yy = yy - bfH/2
    end
    love.graphics.setColor(1,1,1,1)
    love.graphics.draw(img_bottleLine, xx, yy, 0, 1, 1, boW/2, boH/2)


    -- Draw hearts
    xx = x
    yy = y
    local heartW = img_heart:getWidth()
    local heartH = img_heart:getHeight()
    local th = math.cos(0.5+(t*5)) * 0.1
    local ymove = math.cos(0.5+t*5)
    local r = math.cos(t*5) * 5

    if love.system.getOS() == "Horizon" then -- Emulate Love2d origin offset, seems to be off in LovePotion
        xx = xx - heartW/2
        yy = yy - heartH
    end

    love.graphics.draw(img_heart, xx - 20, yy-160, math.rad(-25 - r), 0.6 + th, 0.6 + th, heartW/2, heartH)

    love.graphics.draw(img_heart, xx + 15, yy-140, math.rad(-338 + r/2), 0.3 + th/2, 0.3 + th/2, heartW/2, heartH)

    love.graphics.draw(img_heart, xx + 30, yy-200, math.rad(-347 + (r) ), 0.3 + th/1.5, 0.3 + th/1.5, heartW/2, heartH)

end


-- Helper functions

function lerp(a, b, t)
	return a + (b - a) * t
end

function hexToColor(hex)
	return {
        (1/255)*tonumber(string.sub(hex, 1, 2),16),
        (1/255)*tonumber(string.sub(hex, 3, 4),16),
        (1/255)*tonumber(string.sub(hex, 5, 6),16)
    }
end