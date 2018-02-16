print('OS: ' .. love.system.getOS())
print('Screen: ' .. love.graphics.getWidth() .. 'x' .. love.graphics.getHeight())
print('RendererInfo:')
print(love.graphics.getRendererInfo())
print("LOVE:")
print(love.getVersion())
print('Press + to Quit\n')

function love.load()

end

function printf(str, ...)
	print(string.format(str, ...))
end

function love.gamepadpressed(joystick, button)
	printf("Controller %d: %s was pressed", joystick:getID(), button)
end

function love.gamepadreleased(joystick, button)
	printf("Controller %d: %s was released", joystick:getID(), button)
end

function love.gamepadaxis(joystick, axis, value)
	id = joystick:getID()

	if axis == "leftx" then
		printf("Controller %d left X-Axis: %d", id, value)
	elseif axis == "lefty" then
		printf("Controller %d left Y-Axis: %d", id, value)
	end
end