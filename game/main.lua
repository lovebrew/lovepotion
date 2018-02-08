function love.load()
	print("Hello World!")
	print("OS: " .. love.system.getOS())
	print("Screen: " .. love.graphics.getWidth() .. "x" .. love.graphics.getHeight())
	print("Press + to Exit")


	print("RendererInfo:")
	print(love.graphics.getRendererInfo())
end