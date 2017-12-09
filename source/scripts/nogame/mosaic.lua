Mosaic = class("Mosaic")

function Mosaic:init(screen)
	self.screen = screen

	local mosaic_image = g_images.mosaic[1]

	local sw, sh = mosaic_image:getDimensions()
	local ww, wh

	if self.screen == "bottom" then
		ww, wh = 300, 240
	else
		ww, wh = 400, 240
	end

	local SIZE_X = math.floor(ww / 32 + 2)
	local SIZE_Y = math.floor(wh / 32 + 2) - 2

	local SIZE = SIZE_X * SIZE_Y

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
	love.graphics.setColor(255, 255, 255, 64)
	for idx,piece in ipairs(self.pieces) do
		local ct = 1 - self.color_t
		local c0 = piece.color.prev
		local c1 = piece.color.next
		local r = easeOut(ct, c0[1], c1[1] - c0[1], 1)
		local g = easeOut(ct, c0[2], c1[2] - c0[2], 1)
		local b = easeOut(ct, c0[3], c1[3] - c0[3], 1)

		love.graphics.setColor(r, g, b, 255)
		love.graphics.draw(g_images.mosaic[1], piece.quad, piece.x, piece.y, 0, 1, 1)
	end
	love.graphics.setColor(255, 255, 255, 255)
end