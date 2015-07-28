
local boxx = 50
local boxy = 50

function love.load()

	print('Hello, world!\n')
	print('Top Screen Width: ' .. love.graphics.getWidth())
	print('Top Screen Height: ' .. love.graphics.getHeight())

	love.graphics.setScreen("bottom")

	print(love.graphics.getScreen())

	print('Bottom Screen Width: ' .. love.graphics.getWidth())
	print('Bottom Screen Height: ' .. love.graphics.getHeight())

	love.graphics.setBackgroundColor(0, 255, 0)

end

function love.draw()

	love.graphics.setColor(0, 0, 255)
	love.graphics.rectangle('fill', boxx, boxy, 50, 50)
	love.graphics.setColor(255, 0, 0)
	love.graphics.circle('fill', 200, 150, 50, 50)
	love.graphics.line(50, 30, 200, 50)

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