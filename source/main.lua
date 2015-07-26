local boxx = 50;
local boxy = 50;

function love.load()

	print('Hello, world!')

end

function love.draw()

	love.graphics.setBackgroundColor(0, 255, 0)
	love.graphics.setColor(0, 0, 255)
	love.graphics.rectangle('fill', boxx, boxy, 50, 50)
	love.graphics.setColor(255, 0, 0)
	love.graphics.circle('fill', 200, 150, 50, 50)
	love.graphics.line(50, 30, 200, 50);

end

function love.update(dt)

	if love.keyboard.isDown('cpadleft') then
		boxx = boxx - 1
		print('LEFT')
	end

	if love.keyboard.isDown('cpadright') then
		boxx = boxx + 1
		print('RIGHT')
	end

	if love.keyboard.isDown('cpadup') then
		boxy = boxy - 1
		print('UP')
	end

	if love.keyboard.isDown('cpaddown') then
		boxy = boxy + 1
		print('DOWN')
	end

end