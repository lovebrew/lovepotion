function love.load()
	print('OS: ' .. love.system.getOS())
	print('Screen: ' .. love.graphics.getWidth() .. 'x' .. love.graphics.getHeight())
	print('RendererInfo:')
	print(love.graphics.getRendererInfo())
	print("LOVE:")
	print(love.getVersion())

	print("Save Identity: " .. love.filesystem.getIdentity())
end

function printf(str, ...)
	print(string.format(str, ...))
end

function love.gamepadpressed(id, button)
	printf("Controller %d: %s was pressed", id, button)

	if button == "plus" then
		love.event.quit()
	end
end

function love.gamepadreleased(id, button)
	printf("Controller %d: %s was released", id, button)
end

function love.gamepadaxis(id, axis, value)
	printf("Controller %d %s: %.2f", id, axis, value)
end

function love.touchpressed(id, x, y, dx, dy, pressure)
	printf("Touch pressed at %d, %d", x, y)
end

function love.touchreleased(id, x, y, dx, dy, pressure)
	printf("Touch released at %d, %d", x, y)
end