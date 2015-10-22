local testsound = love.audio.newSource('test_sound.raw')

function love.keypressed(key, isrepeat)

	if key == 'start' then love.event.quit() end

	testsound:play()

end