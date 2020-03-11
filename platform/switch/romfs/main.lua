function love.load()

end

function love.draw()

end

function love.gamepadpressed(joystick, button)
    if button == "start" then
        love.event.quit()
    end
end
