function new_display(x, y, width, height)
    local display = {}

    display.position = {x = x, y = y}
    display.target = love.graphics.newCanvas(width, height)

    function display:getTarget()
        return self.target
    end

    function display:draw()
        love.graphics.draw(self.target, self.position.x, self.position.y)
    end

    return display
end
