local Swoof = class("Swoof")

local PATH = "graphics/swoof"

function Swoof:init()
	self.ears = {love.graphics.newImage(PATH .. "/EarLeft.png"), love.graphics.newImage(PATH .. "/EarRight.png")}
	self.head = love.graphics.newImage(PATH .. "/Head.png")
	self.body = love.graphics.newImage(PATH .. "/Body.png")
	self.tongue = love.graphics.newImage(PATH .. "/Tongue.png")
	
	self.tail = {}
	for i = 1, 4 do
		self.tail[i] = love.graphics.newImage(PATH .. "/Tail" .. i .. ".png")
	end

	self.shadow = love.graphics.newImage(PATH .. "/DogShadow.png")

	self.tailAnim = {1, 2, 3, 4, 3, 2}

	self.x = 548
	self.y = 260

	self.taili = 1
	self.timer = 0
end

function Swoof:update(dt)
	self.timer = self.timer + 24 * dt
	self.taili = self.tailAnim[math.floor(self.timer % #self.tailAnim) + 1]
end

function Swoof:draw()
	--EARS
	local HEADPANT = 0--math.cos(love.timer.getTime() * 12) * 3
	local TONGUEPANT = 0--math.sin(love.timer.getTime() * 12) * 3
	local TAILWAG = 0--math.sin(love.timer.getTime() * 24) * 25

	love.graphics.draw(self.shadow, self.x + (self.head:getWidth() - self.shadow:getWidth()) / 2, self.y + (self.head:getHeight() + self.body:getHeight() / 2) - self.shadow:getHeight() / 2)

	love.graphics.draw(self.ears[1], self.x - self.ears[1]:getWidth() / 2 + 15, (self.y + 30) + HEADPANT)
	love.graphics.draw(self.ears[2], self.x + self.head:getWidth() - 62, (self.y + 30) + HEADPANT)
	
	--TAIL
	love.graphics.draw(self.tail[self.taili], self.x + self.head:getWidth() - 30, self.y + self.head:getHeight() + 30, 0)

	--BODY - 
	love.graphics.draw(self.body, self.x + (self.head:getWidth() - self.body:getWidth()) / 2, self.y + self.head:getHeight() - 60)
	
	--TONGUE
	love.graphics.draw(self.tongue, self.x + (self.head:getWidth() - self.tongue:getWidth()) / 2, (self.y + self.head:getHeight() - 25) + TONGUEPANT)

	--HEAD
	love.graphics.draw(self.head, self.x, self.y + HEADPANT)

	--SHADOW
end

return Swoof()