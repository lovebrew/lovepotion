local MessageBox = require("window_messagebox")

local dt = 0
local messageBox = nil

local function main()
    local normalScreens = love.graphics.getScreens()
    local plainScreens = {"top", "bottom"}

    while true do
        if love.event and love.event.pump then
            love.event.pump()

            for name, a, b, c, d, e, f in love.event.poll() do
                if name == "quit" then
                    return 0
                else
                    messageBox:poll(name, a, b, c, d, e, f)
                end
            end
        end

        if love.timer then
            dt = love.timer.step()
        end

        if messageBox:update(dt) then
            g_windowShown = false
            return messageBox:getPressedButton()
        end

        if love.graphics then
            local screens = love.graphics.get3D() and normalScreens or plainScreens

            for _, screen in ipairs(screens) do
                love.graphics.origin()

                love.graphics.setActiveScreen(screen)
                love.graphics.clear(love.graphics.getBackgroundColor())

                if screen == "bottom" then
                    messageBox:draw()
                end
            end

            love.graphics.present()
        end

        if love.timer then
            love.timer.sleep(0.001)
        end
    end
end

function love.window.showMessageBox(_, text, buttons)
    assert(#buttons > 0 and #buttons <= 2, "Cannot have more than two buttons")

    messageBox = MessageBox(text, buttons)
    g_windowShown = true

    return main()
end
