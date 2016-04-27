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
-- a lot of nasty hacks/patches are used to get it displayed properly on 3DS,
-- but seeing as we are only coding for one platform this shouldn't matter too much.

function love.nogame()

	local math = math

	-- 30log.lua begins
	local function require_30log()
	local assert, pairs, type, tostring, setmetatable = assert, pairs, type, tostring, setmetatable
	local baseMt, _instances, _classes, _class = {}, setmetatable({},{__mode='k'}), setmetatable({},{__mode='k'})
	local function assert_class(class, method) assert(_classes[class], ('Wrong method call. Expected class:%s.'):format(method)) end
	local function deep_copy(t, dest, aType) t = t or {}; local r = dest or {}
	  for k,v in pairs(t) do
	    if aType and type(v)==aType then r[k] = v elseif not aType then
	      if type(v) == 'table' and k ~= "__index" then r[k] = deep_copy(v) else r[k] = v end
	    end
	  end; return r
	end
	local function instantiate(self,...)
	  assert_class(self, 'new(...) or class(...)'); local instance = {class = self}; _instances[instance] = tostring(instance); setmetatable(instance,self)
	  if self.init then if type(self.init) == 'table' then deep_copy(self.init, instance) else self.init(instance, ...) end; end; return instance
	end
	local function extend(self, name, extra_params)
	  assert_class(self, 'extend(...)'); local heir = {}; _classes[heir] = tostring(heir); deep_copy(extra_params, deep_copy(self, heir));
	  heir.name, heir.__index, heir.super = extra_params and extra_params.name or name, heir, self; return setmetatable(heir,self)
	end
	baseMt = { __call = function (self,...) return self:new(...) end, __tostring = function(self,...)
	  if _instances[self] then return ("instance of '%s' (%s)"):format(rawget(self.class,'name') or '?', _instances[self]) end
	  return _classes[self] and ("class '%s' (%s)"):format(rawget(self,'name') or '?',_classes[self]) or self
	end}; _classes[baseMt] = tostring(baseMt); setmetatable(baseMt, {__tostring = baseMt.__tostring})
	local class = {isClass = function(class, ofsuper) local isclass = not not _classes[class]; if ofsuper then return isclass and (class.super == ofsuper) end; return isclass end, isInstance = function(instance, ofclass) 
	    local isinstance = not not _instances[instance]; if ofclass then return isinstance and (instance.class == ofclass) end; return isinstance end}; _class = function(name, attr)
	  local c = deep_copy(attr); c.mixins=setmetatable({},{__mode='k'}); _classes[c] = tostring(c); c.name, c.__tostring, c.__call = name or c.name, baseMt.__tostring, baseMt.__call
	  c.include = function(self,mixin) assert_class(self, 'include(mixin)'); self.mixins[mixin] = true; return deep_copy(mixin, self, 'function') end
	  c.new, c.extend, c.__index, c.includes = instantiate, extend, c, function(self,mixin) assert_class(self,'includes(mixin)') return not not (self.mixins[mixin] or (self.super and self.super:includes(mixin))) end
	  c.extends = function(self, class) assert_class(self, 'extends(class)') local super = self; repeat super = super.super until (super == class or super == nil); return class and (super == class) end
	    return setmetatable(c, baseMt) end; class._DESCRIPTION = '30 lines library for object orientation in Lua'; class._VERSION = '30log v1.0.0'; class._URL = 'http://github.com/Yonaba/30log'; class._LICENSE = 'MIT LICENSE <http://www.opensource.org/licenses/mit-license.php>'
	return setmetatable(class,{__call = function(_,...) return _class(...) end })
	end
	-- 30log.lua ends

	local class = require_30log()

	local Toast = class("Toast")

	function Toast:init()
		self:center()

		self.eyes = {}
		self.eyes.closed_t = 0
		self.eyes.blink_t = 2

		self.look = {}
		self.look.target = { x = 0.2, y = 0.2 }
		self.look.current = { x = 0.2, y = 0.2 }
		self.look.DURATION = 0.5
		self.look.POINTS = {
			{ x = 0.8, y = 0.8 },
			{ x = 0.1, y = 0.1 },
			{ x = 0.8, y = 0.1 },
			{ x = 0.1, y = 0.8 },
		}
		self.look.point = 0
		self.look.point_t = 1
		self.look.t = 0

	end

	local function easeOut(t, b, c, d)
		t = t / d - 1
		return c * (math.pow(t, 3) + 1) + b
	end

	function Toast:center()
		local ww, wh = 300, 240
		self.x = math.floor(ww / 2 / 32) * 32 + 16
		self.y = math.floor(wh / 2 / 32) * 32 + 16
	end

	function Toast:get_look_coordinates()
		local t = self.look.t

		local src = self.look.current
		local dst = self.look.target

		local look_x = easeOut(t, src.x, dst.x - src.x, self.look.DURATION)
		local look_y = easeOut(t, src.y, dst.y - src.y, self.look.DURATION)

		return look_x, look_y
	end

	function Toast:update(dt)
		self.look.t = math.min(self.look.t + dt, self.look.DURATION)
		self.eyes.closed_t = math.max(self.eyes.closed_t - dt, 0)
		self.eyes.blink_t = math.max(self.eyes.blink_t - dt, 0)
		self.look.point_t = math.max(self.look.point_t - dt, 0)

		if self.eyes.blink_t == 0 then
			self:blink()
		end

		if self.look.point_t == 0 then
			self:look_at_next_point()
		end

		local look_x, look_y = self:get_look_coordinates()

		self.offset_x = look_x * 4
		self.offset_y = (1 - look_y) * -4
	end

	function Toast:draw()
		local x = self.x
		local y = self.y

		local look_x, look_y = self:get_look_coordinates()

		love.graphics.draw(g_images.toast.back, x, y, self.r, 1, 1, 50, 50)
		love.graphics.draw(g_images.toast.front, x + self.offset_x, y + self.offset_y, self.r, 1, 1, 50, 50)
		love.graphics.draw(self:get_eyes_image(), x + self.offset_x * 2.5, y + self.offset_y * 2.5, self.r, 1, 1, 50, 50)
		love.graphics.draw(g_images.toast.mouth, x + self.offset_x * 2, y + self.offset_y * 2, self.r, 1, 1, 50, 50)
	end

	function Toast:get_eyes_image()
		if self.eyes.closed_t > 0 then
			return g_images.toast.eyes.closed
		end
		return g_images.toast.eyes.open
	end

	function Toast:blink()
		if self.eyes.closed_t > 0 then
			return
		end
		self.eyes.closed_t = 0.1
		self.eyes.blink_t = self.next_blink()
	end

	function Toast:next_blink()
		return 5 + math.random(0, 3)
	end

	function Toast:look_at(tx, ty)
		local look_x, look_y = self:get_look_coordinates()
		self.look.current.x = look_x
		self.look.current.y = look_y

		self.look.t = 0
		self.look.point_t = 3 + math.random(0, 1)

		self.look.target.x = tx
		self.look.target.y = ty
	end

	function Toast:look_at_next_point()
		self.look.point = self.look.point + 1
		if self.look.point > #self.look.POINTS then
			self.look.point = 1
		end
		local point = self.look.POINTS[self.look.point]
		self:look_at(point.x, point.y)
	end

	local Mosaic = class("Mosaic")

	function Mosaic:init()

		self.screen = love.graphics.getScreen()

		local mosaic_image = g_images.mosaic[1]

		local sw, sh = mosaic_image:getDimensions()
		local ww, wh

		if self.screen == "bottom" then
			ww, wh = 300, 240
		else
			ww, wh = 400, 240
		end

		-- if love.window.getPixelScale() > 1 then
		-- 	mosaic_image = g_images.mosaic[2]
		-- end

		local SIZE_X = math.floor(ww / 32 + 2)
		local SIZE_Y = math.floor(wh / 32 + 2) - 2

		local SIZE = SIZE_X * SIZE_Y

		-- self.batch = love.graphics.newSpriteBatch(mosaic_image, SIZE, "stream")
		self.pieces = {}
		self.color_t = 1
		self.generation = 1

		local COLORS = {}

		for _,color in ipairs({
			{ 240, 240, 240 }, -- WHITE (ish)
			{ 232, 104, 162}, -- PINK
			{ 69, 155, 168 }, -- BLUE
			{ 67, 93, 119 }, -- DARK BLUE
		}) do
			table.insert(COLORS, color)
			table.insert(COLORS, color)
		end

		-- Insert only once. This way it appears half as often.
		table.insert(COLORS, { 220, 239, 113 }) -- LIME

		-- When using the higher-res mosaic sprite sheet, we want to draw its
		-- sprites at the same scale as the regular-resolution one, because
		-- we'll globally love.graphics.scale *everything* by the screen's
		-- pixel density ratio.
		-- We can avoid a lot of Quad scaling by taking advantage of the fact
		-- that Quads use normalized texture coordinates internally - if we use 
		-- the 'source image size' and quad size of the @1x image for the Quads
		-- even when rendering them using the @2x image, it will automatically
		-- scale as expected.
		local QUADS = {
			love.graphics.newQuad(0,  0,  32, 32, sw, sh),
			love.graphics.newQuad(0,  32, 32, 32, sw, sh),
			love.graphics.newQuad(32, 32, 32, 32, sw, sh),
			love.graphics.newQuad(32, 0,  32, 32, sw, sh),
		}

		local exclude_left = math.floor(ww / 2 / 32)
		local exclude_right = exclude_left + 3
		local exclude_top = math.floor(wh / 2 / 32)
		local exclude_bottom = exclude_top + 3
		local exclude_width = exclude_right - exclude_left + 1
		local exclude_height = exclude_bottom - exclude_top + 1
		local exclude_area = exclude_width * exclude_height

		local exclude_center_x = exclude_left + 1.5
		local exclude_center_y = exclude_top + 1.5

		self.generators = {
			function(piece, generation)
				return COLORS[math.random(1, #COLORS)]
			end,
			function(piece, generation)
				return COLORS[1 + (generation + piece.grid_x - piece.grid_y) % #COLORS]
			end,
			function(piece, generation)
				return COLORS[1 + (piece.grid_x + generation) % #COLORS]
			end,
			function(piece, generation)
				local len = generation + math.sqrt(piece.grid_x ^ 2 + piece.grid_y ^ 2)
				return COLORS[1 + math.floor(len) % #COLORS]
			end,
			function(piece, generation)
				local dx = piece.grid_x - exclude_center_x - 1
				local dy = piece.grid_y - exclude_center_y - 1
				local len = generation - math.sqrt(dx ^ 2 + dy ^ 2)
				return COLORS[1 + math.floor(len) % #COLORS]
			end,
			function(piece, generation)
				local dx = math.abs(piece.grid_x - exclude_center_x) - generation
				local dy = math.abs(piece.grid_y - exclude_center_y) - generation
				return COLORS[1 + math.floor(math.max(dx, dy)) % #COLORS]
			end,
		}

		self.generator = self.generators[1]

		local EXCLUDE = {}

		if self.screen == 'bottom' then
			for y = exclude_top,exclude_bottom do
				EXCLUDE[y]  = {}
				for x = exclude_left,exclude_right do
					EXCLUDE[y][x] = true
				end
			end
		end

		for y = 1,SIZE_Y do
			for x = 1,SIZE_X do
				if not EXCLUDE[y] or not EXCLUDE[y][x] then
					local piece = {
						grid_x = x,
						grid_y = y,
						x = (x - 1) * 32,
						y = (y - 1) * 32,
						r = math.random(0, 100) / 100 * math.pi,
						rv = 1,
						color = {},
						quad = QUADS[(x + y) % 4 + 1]
					}

					piece.color.prev = self.generator(piece, self.generation)
					piece.color.next = piece.color.prev
					table.insert(self.pieces, piece)
				end
			end
		end

		local GLYPHS = {
			N = love.graphics.newQuad(0,  64, 32, 32, sw, sh),
			O = love.graphics.newQuad(32, 64, 32, 32, sw, sh),
			G = love.graphics.newQuad(0,  96, 32, 32, sw, sh),
			A = love.graphics.newQuad(32, 96, 32, 32, sw, sh),
			M = love.graphics.newQuad(64, 96, 32, 32, sw, sh),
			E = love.graphics.newQuad(96, 96, 32, 32, sw, sh),

			U = love.graphics.newQuad(64, 0,  32, 32, sw, sh),
			P = love.graphics.newQuad(96, 0,  32, 32, sw, sh),
			o = love.graphics.newQuad(64, 32, 32, 32, sw, sh),
			S = love.graphics.newQuad(96, 32, 32, 32, sw, sh),
			R = love.graphics.newQuad(64, 64, 32, 32, sw, sh),
			T = love.graphics.newQuad(96, 64, 32, 32, sw, sh),
		}

		local INITIAL_TEXT_COLOR = { 240, 240, 240 }

		local put_text = function(str, offset, x, y)
			local idx = offset + SIZE_X * y + x
			for i = 1, #str do
				local c = str:sub(i, i)
				if c ~= " " then
					local piece = self.pieces[idx + i]
					piece.quad = GLYPHS[c]
					piece.r = 0
					piece.rv = 0
					piece.color.prev = INITIAL_TEXT_COLOR
					piece.color.next = INITIAL_TEXT_COLOR
				end
			end
		end

		local text_center_x = math.floor(ww / 2 / 32)

		local no_game_text_offset = SIZE_X * exclude_bottom - exclude_area

		if self.screen == "top" then put_text("No GAME", no_game_text_offset + 1, text_center_x - 2, -2) end
		if self.screen == "bottom" then put_text("SUPERTOAST", 0, text_center_x - 4, exclude_top - 3) end

	end

	function Mosaic:addGeneration()
		self.generation = self.generation + 1
		if self.generation % 5 == 0 then
			if math.random(0, 100) < 60 then
				self.generator = self.generators[math.random(2, #self.generators)]
			else
				self.generator = self.generators[1]
			end
		end
	end

	function Mosaic:update(dt)
		self.color_t = math.max(self.color_t - dt, 0)
		local change_color = self.color_t == 0
		if change_color then
			self.color_t = 1
			self:addGeneration()
		end
		local gen = self.generator
		for idx,piece in ipairs(self.pieces) do
			piece.r = piece.r + piece.rv * dt
			if change_color then
				piece.color.prev = piece.color.next
				piece.color.next = gen(piece, self.generation)
			end
		end
	end

	function Mosaic:draw()
		-- self.batch:clear()
		love.graphics.setColor(255, 255, 255, 64)
		for idx,piece in ipairs(self.pieces) do
			local ct = 1 - self.color_t
			local c0 = piece.color.prev
			local c1 = piece.color.next
			local r = easeOut(ct, c0[1], c1[1] - c0[1], 1)
			local g = easeOut(ct, c0[2], c1[2] - c0[2], 1)
			local b = easeOut(ct, c0[3], c1[3] - c0[3], 1)

			-- self.batch:setColor(r, g, b)
			-- self.batch:add(piece.quad, piece.x, piece.y, piece.r, 1, 1, 16, 16)
			love.graphics.setColor(r, g, b, 255)
			love.graphics.draw(g_images.mosaic[1], piece.quad, piece.x, piece.y, 0, 1, 1)
		end
		love.graphics.setColor(255, 255, 255, 255)
		-- love.graphics.draw(self.batch, 0, 0)
	end

	function love.load()
		love.graphics.setBackgroundColor(136, 193, 206)

		g_images = {}
		g_images.toast = {}
		g_images.toast.back = love.graphics.newImage('nogame:toast_back')
		g_images.toast.front = love.graphics.newImage('nogame:toast_front')
		g_images.toast.eyes = {}
		g_images.toast.eyes.open = love.graphics.newImage('nogame:toast_eyes_open')
		g_images.toast.eyes.closed = love.graphics.newImage('nogame:toast_eyes_closed')
		g_images.toast.mouth = love.graphics.newImage('nogame:toast_mouth')

		g_images.mosaic = {}
		g_images.mosaic[1] = love.graphics.newImage('nogame:mosaic')

		g_entities = {}
		g_entities.toast = Toast()

		love.graphics.setScreen('bottom')
		g_entities.mosaic = Mosaic()

		love.graphics.setScreen('top')
		g_entities.topmosaic = Mosaic()

	end

	function love.update(dt)
		dt = math.min(dt, 1/10)
		g_entities.toast:update(dt)
		g_entities.mosaic:update(dt)
		g_entities.topmosaic:update(dt)
	end

	function love.draw()

		love.graphics.setScreen('bottom')

		love.graphics.push()
		love.graphics.translate(0, 10)
		love.graphics.setColor(255, 255, 255)

		-- love.graphics.push()
		-- love.graphics.scale(love.window.getPixelScale())

		g_entities.mosaic:draw()
		g_entities.toast:draw()
		love.graphics.pop()

		love.graphics.setScreen('top')

		love.graphics.push()
		love.graphics.translate(-5, 10)
		love.graphics.setColor(255, 255, 255)

		g_entities.topmosaic:draw()
		love.graphics.pop()

	end

	function love.keypressed(key)
		if key == "start" then
			love.event.quit()
		end
	end

	function love.mousepressed(x, y, b)
		local tx = x / 300
		local ty = y / 240
		g_entities.toast:look_at(tx, ty)
	end

	function love.mousemoved(x, y)
		if love.mouse.isDown(1) then
			local tx = x / 300
			local ty = y / 240
			g_entities.toast:look_at(tx, ty)
		end
	end

	local last_touch = {time=0, x=0, y=0}

end

love.nogame()