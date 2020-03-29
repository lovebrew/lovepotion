local TEXTURES = {}

local console = (love._console_name == "3DS" and "3ds") or "switch"

TEXTURES.logo = love.graphics.newImage("graphics/" .. console .. "/logo.png")
TEXTURES.text = love.graphics.newImage("graphics/" .. console .. "/text.png")
TEXTURES.textQuads = {}

return TEXTURES
