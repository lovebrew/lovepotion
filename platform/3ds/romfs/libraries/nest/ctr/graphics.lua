window = {}
window.screen = "top"

love.graphics.getDimensions = function()
    if window.screen == "top" then
        return 400, 240
    end
    return 320, 240
end

love.graphics.getWidth = function()
    if window.screen == "top" then
        return 400
    end
    return 320
end

love.graphics.getHeight = function()
    return 320
end
