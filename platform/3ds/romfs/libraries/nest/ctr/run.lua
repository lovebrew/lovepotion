return function()
    if love.load then love.load(love.arg.parseGameArguments(arg), arg) end

    -- We don't want the first frame's dt to include time taken by love.load.
    if love.timer then love.timer.step() end

    local dt = 0

    local screens =
    {
        new_display(0, 0, 400, 240),
        new_display(40, 240, 320, 240)
    }

    -- Main loop time.
    return function()
        -- Process events.
        if love.event then
            love.event.pump()
            for name, a,b,c,d,e,f in love.event.poll() do
                if name == "quit" then
                    if not love.quit or not love.quit() then
                        return a or 0
                    end
                end
                love.handlers[name](a,b,c,d,e,f)
            end
        end

        -- Update dt, as we'll be passing it to update
        if love.timer then dt = love.timer.step() end

        -- Call update and draw
        if love.update then love.update(dt) end -- will pass 0 if love.timer is disabled

        if love.graphics and love.graphics.isActive() then
            for i, v in ipairs(screens) do
                local target = v:getTarget()

                target:renderTo(function()
                    love.graphics.clear(love.graphics.getBackgroundColor())
                    local screen = (i == 1 and "top") or "bottom"
                    window.screen = screen

                    love.draw(screen)
                end)

                v:draw()
            end
            love.graphics.present()
        end

        if love.timer then love.timer.sleep(0.001) end
    end
end
