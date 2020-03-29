local module = (love._console_name == "3DS" and "3ds") or "switch"
local CONSOLE    = require("data." .. module)

local konami =
{
    "dpup", "dpup", "dpdown", "dpdown",
    "dpleft", "dpright", "dpleft", "dpright",
    "b", "a", "start"
}

local konamiIndex = 0

function love.load()
    texture = love.graphics.newImage("graphics/" .. module .. "/logo.png")
    textImage = love.graphics.newImage("graphics/" .. module ..  "/text.png")
    source = love.audio.newSource("audio/love.ogg", "stream")

    CONSOLE:initialize()
end

function love.draw(screen)
    CONSOLE:draw(screen)
end

function love.gamepadpressed(joy, button)
    if konami[konamiIndex + 1] == button then
        if not source:isPlaying() then
            konamiIndex = math.min(konamiIndex + 1, #konami)

            if konamiIndex == #konami then
                love.audio.play(source)
                konamiIndex = 1
            end
        end
    else
        if konamiIndex > 1 then
            konamiIndex = 1
        end

        if key == "start" and konamiIndex <= 1 then
            love.event.quit()
        end
    end
end
