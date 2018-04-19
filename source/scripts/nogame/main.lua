class = require '30log'
require 'cloud'
doggo = require 'dog'

function love.load()
	love.graphics.setBackgroundColor(0.27, 0.35, 0.39)

	love.math.setRandomSeed(os.time())

	clouds =
	{
		Cloud(1, love.graphics.getWidth() * 0.10, love.graphics.getHeight() * 0.25),
		Cloud(2, love.graphics.getWidth() * 0.50 - 86, love.graphics.getHeight() * 0.05),
		Cloud(3, love.graphics.getWidth() * 0.70, love.graphics.getHeight() * 0.30)
	}

	bowlImage = love.graphics.newImage("graphics/swoof/Bowl.png")

	love.filesystem.write("test", table.concat({love.graphics.getBackgroundColor()}, "\n"))
end

function love.update(dt)
	dt = math.min(1 / 30, dt)
	
	doggo:update(dt)
end

function love.draw()
	for _, v in ipairs(clouds) do
		v:draw()
	end

	doggo:draw()

	love.graphics.draw(bowlImage, 532, 540)
end

function love.keypressed(key)
	if key == "escape" then
		love.event.quit()
	end
end