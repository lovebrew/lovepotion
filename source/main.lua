
local boxx = 50
local boxy = 50

function love.load()

	print('Hello, world!\n')
	print('Top Screen Width: ' .. love.graphics.getWidth())
	print('Top Screen Height: ' .. love.graphics.getHeight())

	love.graphics.setScreen("bottom")

	print('')

	print('Bottom Screen Width: ' .. love.graphics.getWidth())
	print('Bottom Screen Height: ' .. love.graphics.getHeight())

	love.graphics.setScreen("top")

end

function love.draw()

	love.graphics.print("Hello World!", 10, 10)
	love.graphics.print("FPS: " .. math.ceil(love.timer.getFPS()), 10, 26)

end

function love.update(dt)

	if love.keyboard.isDown('cpadleft') or love.keyboard.isDown('left') then
		boxx = boxx - 100 * dt
		print('LEFT')
	end

	if love.keyboard.isDown('cpadright') or love.keyboard.isDown('right') then
		boxx = boxx + 100 * dt
		print('RIGHT')
	end

	if love.keyboard.isDown('cpadup') or love.keyboard.isDown('up') then
		boxy = boxy - 100 * dt
		print('UP')
	end

	if love.keyboard.isDown('cpaddown') or love.keyboard.isDown('down') then
		boxy = boxy + 100 * dt
		print('DOWN')
	end

end