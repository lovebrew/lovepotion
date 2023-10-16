R"luastring"--(
local function main(...)
    local screens = love.graphics.getScreens()
    local delta_time = 0

    local messagebox = require("window_messagebox")
    local message_box = messagebox.new(...)

    while true do
        if love.event and love.event.pump then
            love.event.pump()

            for name, a, b, c, d, e, f in love.event.poll() do
                if name == "quit" then
                    return 0
                else
                    local result = message_box:poll(name, a, b, c, d, e, f)
                    if result then
                        g_windowShown = false
                        return result
                    end
                end
            end
        end

        if love.timer then
            delta_time = love.timer.step()
        end

        message_box:update(delta_time)

        if love.graphics then
            for _, screen in ipairs(screens) do
                love.graphics.origin()

                love.graphics.setActiveScreen(screen)
                love.graphics.clear(love.graphics.getBackgroundColor())

                if screen == "bottom" then
                    message_box:draw()
                end
            end

            love.graphics.present()
        end

        if love.timer then
            love.timer.sleep(0.001)
        end
    end
end

function love.window.showMessageBox(title, text, buttons)
    g_windowShown = true

    return main(title, text, buttons)
end
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
