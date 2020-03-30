local NX = {}

local COLORS     = require("data.colors")
local BACKGROUND = require("data.background")
local TEXTURES   = require("data.textures")

local width, height = 1280, 720

function NX:initialize()
    for i = 1, #"nogame" do
        TEXTURES.textQuads[string.sub("nogame", i, i)] = love.graphics.newQuad((i - 1) * 140, 0, 140, 128, TEXTURES.text)
    end

    self.background = BACKGROUND:new(1280, 720)
end

local function draw_glyph(text, x, y, r, ox, oy)
    love.graphics.rectangle('line', x, y, 140, 128)
    love.graphics.draw(TEXTURES.text, TEXTURES.textQuads[text], x + (140 / 2), y + (128 / 2), r, 1, 1, ox, oy)
end

function NX:draw()
    love.graphics.setColor(COLORS.PINK)
    love.graphics.polygon("fill", self.background[1])

    love.graphics.setColor(COLORS.BLUE)
    love.graphics.polygon("fill", self.background[2])

    love.graphics.setColor(COLORS.WHITE)
    love.graphics.draw(TEXTURES.logo, (width - TEXTURES.logo:getWidth()) / 2 - 35, (height - TEXTURES.logo:getHeight()) / 2)

    local rotation = math.rad(-26)
    local ox, oy = 70, 64

    draw_glyph("n", width * 0.100, height * 0.660, rotation, ox, oy)
    draw_glyph("o", width * 0.200, height * 0.560, rotation, ox, oy)

    draw_glyph("g", width * 0.600, height * 0.350, rotation, ox, oy)
    draw_glyph("a", width * 0.700, height * 0.250, rotation, ox, oy)
    draw_glyph("m", width * 0.800, height * 0.150, rotation, ox, oy)
    draw_glyph("e", width * 0.900, height * 0.050, rotation, ox, oy)
    -- love.graphics.draw(TEXTURES.text, TEXTURES.textQuads["n"], width * 0.100, y, rotation, 1, 1, ox, oy)
    -- love.graphics.draw(TEXTURES.text, TEXTURES.textQuads["o"], width * 0.200, y, rotation, 1, 1, ox, oy)
end

return NX
