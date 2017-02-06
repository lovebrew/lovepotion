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

print('LovePotion 1.0.9 BETA (LOVE for 3DS)\n')

package.path = './?.lua;./?/init.lua'
package.cpath = './?.lua;./?/init.lua'

_defaultFont_ = love.graphics.newFont()
love.graphics.setFont(_defaultFont_)

--SET THE GAME'S IDENTITY: REQUIRED FOR SAVE DATA
love.filesystem.setIdentity("SuperGame")

function love.createhandlers()

	-- Standard callback handlers.
	love.handlers = setmetatable({
		keypressed = function (b,s,r)
			if love.keypressed then return love.keypressed(b,s,r) end
		end,
		keyreleased = function (b,s)
			if love.keyreleased then return love.keyreleased(b,s) end
		end,
		textinput = function (t)
			if love.textinput then return love.textinput(t) end
		end,
		textedited = function (t,s,l)
			if love.textedited then return love.textedited(t,s,l) end
		end,
		mousemoved = function (x,y,dx,dy,t)
			if love.mousemoved then return love.mousemoved(x,y,dx,dy,t) end
		end,
		mousepressed = function (x,y,b,t)
			if love.mousepressed then return love.mousepressed(x,y,b,t) end
		end,
		mousereleased = function (x,y,b,t)
			if love.mousereleased then return love.mousereleased(x,y,b,t) end
		end,
		wheelmoved = function (x,y)
			if love.wheelmoved then return love.wheelmoved(x,y) end
		end,
		touchpressed = function (id,x,y,dx,dy,p)
			if love.touchpressed then return love.touchpressed(id,x,y,dx,dy,p) end
		end,
		touchreleased = function (id,x,y,dx,dy,p)
			if love.touchreleased then return love.touchreleased(id,x,y,dx,dy,p) end
		end,
		touchmoved = function (id,x,y,dx,dy,p)
			if love.touchmoved then return love.touchmoved(id,x,y,dx,dy,p) end
		end,
		joystickpressed = function (j,b)
			if love.joystickpressed then return love.joystickpressed(j,b) end
		end,
		joystickreleased = function (j,b)
			if love.joystickreleased then return love.joystickreleased(j,b) end
		end,
		joystickaxis = function (j,a,v)
			if love.joystickaxis then return love.joystickaxis(j,a,v) end
		end,
		joystickhat = function (j,h,v)
			if love.joystickhat then return love.joystickhat(j,h,v) end
		end,
		gamepadpressed = function (j,b)
			if love.gamepadpressed then return love.gamepadpressed(j,b) end
		end,
		gamepadreleased = function (j,b)
			if love.gamepadreleased then return love.gamepadreleased(j,b) end
		end,
		gamepadaxis = function (j,a,v)
			if love.gamepadaxis then return love.gamepadaxis(j,a,v) end
		end,
		joystickadded = function (j)
			if love.joystickadded then return love.joystickadded(j) end
		end,
		joystickremoved = function (j)
			if love.joystickremoved then return love.joystickremoved(j) end
		end,
		focus = function (f)
			if love.focus then return love.focus(f) end
		end,
		mousefocus = function (f)
			if love.mousefocus then return love.mousefocus(f) end
		end,
		visible = function (v)
			if love.visible then return love.visible(v) end
		end,
		quit = function ()
			return
		end,
		threaderror = function (t, err)
			if love.threaderror then return love.threaderror(t, err) end
		end,
		resize = function (w, h)
			if love.resize then return love.resize(w, h) end
		end,
		filedropped = function (f)
			if love.filedropped then return love.filedropped(f) end
		end,
		directorydropped = function (dir)
			if love.directorydropped then return love.directorydropped(dir) end
		end,
		lowmemory = function ()
			collectgarbage()
			if love.lowmemory then return love.lowmemory() end
		end,
	}, {
		__index = function(self, name)
			error('Unknown event: ' .. name)
		end,
	})

end

love.createhandlers()

function love.errhand(msg)
	love.graphics.set3D(false)
	love.audio.stop()
	love.graphics.setBackgroundColor(89, 157, 220)
	love.graphics.setScreen('top')
	love.graphics.setFont(_defaultFont_)
	love.graphics.setColor(255, 255, 255, 255)
	love.graphics.print('Oops, a Lua error has occured', 25, 25)
	love.graphics.print('Press Start to quit', 25, 40)
	love.graphics.printf(msg, 25, 70, love.graphics.getWidth() - 50)
end