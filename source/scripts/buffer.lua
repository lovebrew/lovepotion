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

function love.gamepadpressed(joystick, button)
	printf("Controller %d: %s was pressed", joystick:getID(), button)

	if button == "plus" then
		love.event.quit()
	end
end

function love.gamepadreleased(joystick, button)
	printf("Controller %d: %s was released", joystick:getID(), button)
end

function love.gamepadaxis(joystick, axis, value)
	id = joystick:getID()

	printf("Controller %d %s: %.2f", id, axis, value)
end