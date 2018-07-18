function love.load()
	print("Setting seed!")
	love.math.setRandomSeed(os.time())

	print("RNG (default): " .. love.math.random())
	print("RNG (ranged): " .. love.math.random(0, 10))
end

function love.update(dt)

end