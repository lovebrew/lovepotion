function love.draw()

	love.graphics.print("Replace me in game/main.lua.", 0, 0)

end

function love.keypressed(key, isrepeat)

	if key == "start" then love.event.quit() end

end