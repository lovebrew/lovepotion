-- This code is licensed under the MIT Open Source License.

-- Copyright (c) 2016 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:

-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.

-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

-- This is a bit messy
-- But it means we can move stuff out of main.c
package.path = './?.lua;./?/init.lua'
package.cpath = './?.lua;./?/init.lua'

_defaultFont_ = love.graphics.newFont()
love.graphics.setFont(_defaultFont_)

local config = 
{
	console = false,
	version = "1.0.9",
	identity = "SuperGame"
}

if love.filesystem.isFile("conf.lua") then
	success, err = pcall(require, 'conf')

	if success and love.conf then
		love.conf(config)

		if config.console then
			love.enableConsole()
		end
	end
end

love.filesystem.setIdentity(config.identity)
--love.enableScreen(config.window.top, config.window.bottom);

local function wrapText(text, x, y, len)
	local width = 0
	local originY = y
	local ret = ""

	for word in text:gmatch("%S+% ?") do
		width = width + _defaultFont_:getWidth(word)
		if x + width >= len then
			word = "\n" .. word 
			originY = originY + 18
			width = 0
		end

		ret = ret .. word
	end

	love.graphics.print(ret, x, y)
end

function love.errhand(message)
	message = message:gsub("^(./)", "")

	love.graphics.set3D(false)

	love.audio.stop()

	love.graphics.setBackgroundColor(66, 165, 245)
	love.graphics.clear()
	
	local function draw()
		love.graphics.setScreen('top')

		love.graphics.setFont(_defaultFont_)
		love.graphics.setColor(255, 255, 255, 255)

		love.graphics.print("Error:", 16, 30)
		wrapText(message, 16, 58, 325)

		love.graphics.print("Press Start to quit", 16, love.graphics.getHeight() - 30)

		if love.keyboard.isDown("start") then
			love.quit()
		end

		love.graphics.present()
	end

	draw()

	while true do
		love.scan()

		love.timer.sleep(0.001)

		if love.keyboard.isDown("start") then
			break
		end
	end
	love.quit()
end

function love.createhandlers()
	-- Standard callback handlers.
	love.handlers = setmetatable({
		keypressed = function (key)
			if love.keypressed then 
				return love.keypressed(key) 
			end
		end,
		keyreleased = function (key)
			if love.keyreleased then 
				return love.keyreleased(key) 
			end
		end,
		mousemoved = function (x,y,dx,dy,t)
			if love.mousemoved then 
				return love.mousemoved(x,y,dx,dy,t) 
			end
		end,
		mousepressed = function (x, y, button)
			if love.mousepressed then 
				return love.mousepressed(x, y, button) 
			end
		end,
		mousereleased = function (x, y, button)
			if love.mousereleased then 
				return love.mousereleased(x, y, button)
			end
		end,
		joystickpressed = function (joystick, button)
			if love.joystickpressed then 
				return love.joystickpressed(joystick, button)
			end
		end,
		joystickreleased = function (joystick, button)
			if love.joystickreleased then 
				return love.joystickreleased(joystick, button)
			end
		end,
		joystickaxis = function (joystick, axis, value)
			if love.joystickaxis then 
				return love.joystickaxis(joystick, axis, value)
			end
		end,
		joystickhat = function (joystick, hat, value)
			if love.joystickhat then 
				return love.joystickhat(joystick, hat, value) 
			end
		end,
		gamepadpressed = function (joystick, button)
			if love.gamepadpressed then 
				return love.gamepadpressed(joystick, button)
			end
		end,
		gamepadreleased = function (joystick, button)
			if love.gamepadreleased then 
				return love.gamepadreleased(joystick, button)
			end
		end,
		gamepadaxis = function (joystick, axis, value)
			if love.gamepadaxis then 
				return love.gamepadaxis(joystick, axis, value)
			end
		end,
		textinput = function(text)
			if love.textinput then 
				return love.textinput(text) 
			end
		end,
		focus = function (focus)
			if love.focus then 
				return love.focus(focus) 
			end
		end,
		visible = function (visible)
			if love.visible then 
				return love.visible(visible) 
			end
		end,
		quit = function ()
			return
		end,
		threaderror = function (t, err)
			if love.threaderror then return love.threaderror(t, err) end
		end,
		lowmemory = function ()
			collectgarbage()
			if love.lowmemory then return love.lowmemory() end
		end
	}, {
		__index = function(self, name)
			error('Unknown event: ' .. name)
		end,
	})

end

love.createhandlers()

if love.filesystem.isFile("main.lua") then
	require 'main'
else
	love.nogame()
end

if love.timer then
	love.timer.step()
end