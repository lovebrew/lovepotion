function love.load()
	print("OS: " .. love.system.getOS())
	print("Screen: " .. love.graphics.getWidth() .. "x" .. love.graphics.getHeight())

	local info = {love.graphics.getRendererInfo()}
	print("RendererInfo: " .. table.concat(info, " "))

	print("Press + to Exit")
end

function love.gamepadaxis(joystick, axis, value)
	print(axis, value)
end