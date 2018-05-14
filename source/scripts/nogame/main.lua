--[[
Copyright (c) 2006-2015 LOVE Development Team

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

-- Edited version of the actual nogame screen.

local last_touch = {time = 0, x = 0, y = 0}
--[[local pass = {"up", "up", "down", "down", "left", "right", "left", "right", "b", "a"}
local passTimeout = 0
local passMaxDelay = 0.5]]

class = require '30log'
require 'toast'
require 'mosaic'

function love.load()
	love.graphics.setBackgroundColor(136, 193, 206)

	g_images = {}
	g_images.toast = {}
	g_images.toast.back = love.graphics.newImage('toast_back.png')
	g_images.toast.front = love.graphics.newImage('toast_front.png')
	g_images.toast.eyes = {}
	g_images.toast.eyes.open = love.graphics.newImage('toast_eyes_open.png')
	g_images.toast.eyes.closed = love.graphics.newImage('toast_eyes_closed.png')
	g_images.toast.mouth = love.graphics.newImage('toast_mouth.png')

	g_images.mosaic = {}
	g_images.mosaic[1] = love.graphics.newImage('mosaic.png')

	toast = Toast:new()
	mosaic = Mosaic:new('bottom')
	topMosaic = Mosaic:new('top')

	passIndex = 1

	--loveSound = love.audio.newSource("love.ogg", "stream")
end

function love.update(dt)
	dt = math.min(dt, 1/10)

	toast:update(dt)
	mosaic:update(dt)
	topMosaic:update(dt)

	--[[if not loveSound:isPlaying() then
		if passIndex > 1 then
			passTimeout = passTimeout + dt
			if passTimeout > passMaxDelay then
				passIndex = 1
				passTimeout = 0
			end
		end
	end]]
end

function love.draw()
	love.graphics.setScreen('top')

	love.graphics.push()

	love.graphics.translate(-5, 8)
	love.graphics.setColor(255, 255, 255)
	topMosaic:draw()

	love.graphics.pop()

	love.graphics.setScreen('bottom')

	love.graphics.push()

	love.graphics.translate(0, 8)
	love.graphics.setColor(255, 255, 255)
	mosaic:draw()
	toast:draw()

	love.graphics.pop()
end

function love.keypressed(key)
	--[[if not loveSound:isPlaying() then
		if key == pass[passIndex] then
			passIndex = math.min(passIndex + 1, #pass)
			passTimeout = 0

			if passIndex == #pass then
				loveSound:play()
				passIndex = 1
			end
			return
		else
			passIndex = 1
		end
	end]]

	if key == "start" then
		love.event.quit()
	end
end

function love.mousepressed(x, y, b)
	local tx = x / 320
	local ty = y / 240
	toast:look_at(tx, ty)
end

function love.mousemoved(x, y)
	if love.mouse.isDown(1) then
		local tx = x / 320
		local ty = y / 240
		toast:look_at(tx, ty)
	end
end

function easeOut(t, b, c, d)
	t = t / d - 1
	return c * (math.pow(t, 3) + 1) + b
end